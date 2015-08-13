; AVR 8-bit microcontroller Threefish primitive -- http://www.skein-hash.info/
; Copyright © 2010 Jörg Walter <info@syntax-k.de>
;
; This library is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program.  If not, see <http://www.gnu.org/licenses/>.

.include "threefish_macros.s"

.altmacro


; add current subkey, keep threefish_round pointers up to date
; Assumes that r31 has already been stored in threefish_tmp.
; Omits certain operations if this is the last addition.
; interrupts must be disabled when calling this
.macro add_subkey r1b0:req, r1b1:req, r1b2:req, r1b3:req, r1b4:req, r1b5:req, r1b6:req, r1b7:req, r2b0:req, r2b1:req, r2b2:req, r2b3:req, r2b4:req, r2b5:req, r2b6:req, r2b7:req, r3b0:req, r3b1:req, r3b2:req, r3b3:req, r3b4:req, r3b5:req, r3b6:req, r3b7:req, r4b0:req, r4b1:req, r4b2:req, r4b3:req, r4b4:req, r4b5:req, r4b6:req, r4b7:req, last
	temp_ld r31, threefish_round_key
	out stack_lo, r31

	add64_stack r31, r1b0, r1b1, r1b2, r1b3, r1b4, r1b5, r1b6, r1b7
	temp_ld r31, threefish_tmp
	temp_st threefish_tmp, r16
	check_wrap r16, key
	.ifb last
		temp_st threefish_round_key, r16
	.endif

	add64_stack r16, r2b0, r2b1, r2b2, r2b3, r2b4, r2b5, r2b6, r2b7
	check_wrap r16, key
	add64_stack r16, r3b0, r3b1, r3b2, r3b3, r3b4, r3b5, r3b6, r3b7
	check_wrap r16, key
	add64_stack r16, r4b0, r4b1, r4b2, r4b3, r4b4, r4b5, r4b6, r4b7

	temp_ld r16, threefish_round_tweak
	out stack_lo, r16
	add64_stack r16, r2b0, r2b1, r2b2, r2b3, r2b4, r2b5, r2b6, r2b7
	check_wrap r16, tweak
	.ifb last
		temp_st threefish_round_tweak, r16
	.endif
	add64_stack r16, r3b0, r3b1, r3b2, r3b3, r3b4, r3b5, r3b6, r3b7

	temp_ld r16, threefish_round
	inc r16
	.ifb last
		temp_st threefish_round, r16
	.endif
	add64_8 r4b0, r4b1, r4b2, r4b3, r4b4, r4b5, r4b6, r4b7, r16
	sp_reg r16
	sp_goto r16, return_address

	.ifb last
		temp_ld r16, threefish_tmp
	.endif
.endm

; MIX operation
.macro MIX j:req, rot:req
	regcall add64, w0j&j, w1j&j
	; The fourth arg influences how register renaming is done in some cases.
	; This particular value makes threefish_256_enc_8threefish_rounds have the same
	; register layout on exit as it had on call.
	rotate 1, j, rot, 1-j
	regcall eor64, w1j&j, w0j&j
.endm


; 8 threefish_rounds of threefish
; Takes internal state in r0-r31, returns it in r0-r31 in the
; exact same layout (see MIX above).
; Assumes that r31 has already been stored in threefish_tmp and will return
; with r31 stored in threefish_tmp.
threefish_256_enc_8threefish_rounds:
	sp_init return_address-1
	reginit
	regcall add_subkey, w0j0, w1j0, w0j1, w1j1
	enable_int
	MIX 0, 14
	MIX 1, 16
	permute 0, 3, 2, 1
	MIX 0, 52
	MIX 1, 57
	permute 0, 3, 2, 1
	MIX 0, 23
	MIX 1, 40
	permute 0, 3, 2, 1
	MIX 0, 5
	MIX 1, 37
	permute 0, 3, 2, 1
	temp_st threefish_tmp, r31
	disable_int r31
	regcall add_subkey, w0j0, w1j0, w0j1, w1j1
	enable_int
	MIX 0, 25
	MIX 1, 33
	permute 0, 3, 2, 1
	MIX 0, 46
	MIX 1, 12
	permute 0, 3, 2, 1
	MIX 0, 58
	MIX 1, 22
	permute 0, 3, 2, 1
	MIX 0, 32
	MIX 1, 32
	permute 0, 3, 2, 1
	temp_st threefish_tmp, r31
	disable_int r31
	sp_reg r31
	ret

; Global entry point, AVR-GCC/AVR-libc C ABI
; This is NOT reentrant (which would be of limited use, if at all, on an AVR)
; r24, r25 = address of params struct, aligned to 256 bytes
; minimal-state variant:
; r8 - r23 = tweak words (r8 = word 0 lsb, r23 = word 1 msb)
.global threefish_256_enc
	.type threefish_256_enc, @function
threefish_256_enc:
	call threefish_setup
	setup_key_schedule 0, -1
	.rept 9
		call threefish_256_enc_8threefish_rounds
	.endr
	regcall add_subkey, w0j0, w1j0, w0j1, w1j1, last
	jmp threefish_finish

threefish_size_enc:
