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

.include "stack_pointer.s"
.include "math.s"
.include "threefish_registers.s"

.altmacro

; Implementation of the Threefish encryption algorithm as described in
; the specification of the Skein hashing algorithm.
;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; Customization ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; You can save 200 cycles execution time by using unused I/O registers.
; Choose 4 locations that are read-write, have no unwanted side effects,
; and are unused by your code during de-/encryption; they will be
; clobbered. Addresses must lie between 0x20..0x5f (registers 0x00-0x3f).
;threefish_tmp         = 0x20+0x09 ; UBRR0L
;threefish_round       = 0x20+0x23 ; OCR2
;threefish_round_key   = 0x20+0x32 ; TCNT0
;threefish_round_tweak = 0x20+0x31 ; OCR0

; Set this to a decent value if you can't disable interrupts while encrypting.
; decent value = sizeof(your automatic variables) + 8 + 2*recursion depth
; minimum value = 1
stack_size  = 4

; probably no need customize these constants for your device, should be the
; same everywhere
stack_hi = 0x3e ; SPH
stack_lo = 0x3d ; SPL
sreg     = 0x3f ; SREG
intflag  = 7

;;;;;;;;;;;;;;;;;;;;;;;;;;;; Customization End ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 

.ifdef threefish_tmp
.include "temp_storage.s"
.else
.comm threefish_tmp, 1
.comm threefish_round, 1
.comm threefish_round_key, 1
.comm threefish_round_tweak, 1
.macro temp_st addr:req, reg:req
	sts addr, reg
.endm

.macro temp_ld reg:req, addr:req
	lds reg, addr
.endm
.endif

; Parameter structure. C equivalent:
;
; struct {
;	char stack[stack_size];
;	char reserved[5];
;	char key[32];
;	char text[32];
; } __attribute((align(256)))__
;
.struct stack_size+4
	new_stack_top:
	key_start:
	key:

.struct key+32
	plaintext:
	key_extra:

.struct key_extra+8
	key_end:
	tweak_start:
	tweak:

.struct tweak+16
	tweak_extra:

.struct tweak_extra+8
	tweak_end:
	plaintext_end:


.struct new_stack_top-2
	old_stack_pointer:

.struct old_stack_pointer-2
	return_address:

.text


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; Execution

; wrap word pointer if top of key/tweak words is reached
.macro check_wrap reg:req, type:req, last
	LOCAL nowrap
	in reg, stack_lo
	cpi reg, type&_end-1
	brne nowrap
	ldi reg, type&_start-1
	.ifb last
		out stack_lo, reg
	.endif
nowrap:
.endm

; setup temporary variables holding the current key schedule offset
.macro setup_key_schedule for:req, offset:req
	ldi r31, for/4+offset
	temp_st threefish_round, r31
	ldi r31, key-1+8*((for/4)%5)
	temp_st threefish_round_key, r31
	ldi r31, tweak-1+8*((for/4)%3)
	temp_st threefish_round_tweak, r31
.endm

; store former I-flag in T while we need to disable interrupts
.macro disable_int reg:req
	in reg, sreg
	bst reg, intflag
	cli
.endm

; restore former I-flag from T
.macro enable_int
	LOCAL noint
	brtc noint
	sei
noint:
.endm
