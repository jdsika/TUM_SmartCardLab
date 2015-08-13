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



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; Initialization

; save registers on stack as required by the ABI
.macro save_context
; fetch return address, move to new stack
	pop r30
	pop r31
	.irp regno, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 28, 29
		push regno
	.endr
.endm

; setup the new stack pointer (managed by stack_pointer.s)
.macro setup_storage
; save old stack pointer, part 1
	in r26, stack_lo
	in r27, stack_hi
; setup new stack pointer
	out stack_hi, r25
	sp_init 0
; save old stack pointer
	sp_goto r24, new_stack_top
	sp_st ,,r26, r27
; save return address on new stack
	sp_st ,,r30, r31
.endm

; prepare extra key word
.macro setup_key
	ldi r23, 0x1b
	ldi r22, 0xd1
	ldi r21, 0x1b
	ldi r20, 0xda
	ldi r19, 0xa9
	ldi r18, 0xfc
	ldi r17, 0x1a
	ldi r16, 0x22

	sp_goto r24, key
	.rept 4
		.irp regno, 16, 17, 18, 19, 20, 21, 22, 23
			sp_ld ,, r24
			eor regno, r24
		.endr
	.endr

	sbiw r30, 24
	sp_reg r24
	sp_goto r24, key_end
	.irp reg, 23, 22, 21, 20, 19, 18, 17, 16
		ld r24, -Z
		sp_st ,, reg
		mov reg, r24
	.endr
	temp_ld r30, threefish_round
	temp_ld r24, threefish_round_key
.endm

; prepare extra tweak word
; there is a lot of pointer juggling going on in order
; to support in-place and in-param en-/decryption
.macro setup_tweak
	adiw r26, 21
	ld r28, X+
	ld r29, X
	adiw r28, 24

	.irp regno, 7, 6, 5, 4, 3, 2, 1, 0, 31, 30
		ld regno, -Y
	.endr

	temp_st threefish_round, r30
	temp_st threefish_tmp, r31
	movw r30, r28

	.irp regno, 29, 28, 27, 26, 25, 24
		ld regno, -Z
	.endr

	temp_st threefish_round_key, r24
	sp_reg r24
	sp_st r24, tweak, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23

	.irp regno, 16, 17, 18, 19, 20, 21, 22, 23
		eor regno, regno-8
	.endr

	adiw r30, 16

	.irp regno, 8, 9, 10, 11, 12, 13, 14, 15
		ld regno, Z+
	.endr

	sp_st r24, tweak_extra, 16, 17, 18, 19, 20, 21, 22, 23
.endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; Cleanup

; store plain/ciphertext, overwriting the plaintext
.macro save_text
	LOCAL regno
	temp_ld r16, threefish_tmp
	temp_st threefish_round, r30
	temp_st threefish_tmp, r31
	temp_st threefish_round_key, r24
	sp_reg r31
	sp_ld r31, old_stack_pointer, 30, 31
	adiw r30, 23
	ld r24, Z+
	ld r31, Z
	mov r30, r24
	temp_ld r24, threefish_round_key
	regno = 16
	.rept 14
		st Z+, regno
		regno = regno+1
	.endr
	temp_ld r28, threefish_round
	temp_ld r29, threefish_tmp
	st Z+, r28
	st Z+, r29
	regno = 0
	.rept 16
		st Z+, regno
		regno = regno+1
	.endr
.endm

; restore old stack pointer
.macro restore_storage
	sp_reg r16
	sp_goto r16, old_stack_pointer
	sp_ld ,, r26, r27
	out stack_lo, r26
	out stack_hi, r27
.endm

; restore registers from stack as required by the ABI
.macro restore_context
	.irp regno, 29, 28, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2
		pop regno
	.endr
.endm

.global threefish_setup
	.type threefish_setup, @function
threefish_setup:
	reginit
	save_context
	disable_int r29
	setup_storage
	setup_tweak
	setup_key
	sp_reg r31
	sp_goto r31, return_address
	ret

.global threefish_finish
	.type threefish_finish, @function
threefish_finish:
	save_text
	restore_storage
	restore_context
	clr r1
	enable_int
	ret

threefish_size_common:
