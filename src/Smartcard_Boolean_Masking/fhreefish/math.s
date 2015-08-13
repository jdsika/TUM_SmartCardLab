; AVR 8-bit microcontroller 64 bit math assembler macros
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

.altmacro

.macro addi reg:req, K:req
	subi reg, -(\K)
.endm

.macro add64 o0:req, o1:req, o2:req, o3:req, o4:req, o5:req, o6:req, o7:req, i0:req, i1:req, i2:req, i3:req, i4:req, i5:req, i6:req, i7:req
	add o0, i0
	adc o1, i1
	adc o2, i2
	adc o3, i3
	adc o4, i4
	adc o5, i5
	adc o6, i6
	adc o7, i7
.endm

.macro sub64 o0:req, o1:req, o2:req, o3:req, o4:req, o5:req, o6:req, o7:req, i0:req, i1:req, i2:req, i3:req, i4:req, i5:req, i6:req, i7:req
	sub o0, i0
	sbc o1, i1
	sbc o2, i2
	sbc o3, i3
	sbc o4, i4
	sbc o5, i5
	sbc o6, i6
	sbc o7, i7
.endm

.macro add64_stack tmp:req, o0:req, o1:req, o2:req, o3:req, o4:req, o5:req, o6:req, o7:req
	sp_ld ,, tmp
	add o0, tmp
	sp_ld ,, tmp
	adc o1, tmp
	sp_ld ,, tmp
	adc o2, tmp
	sp_ld ,, tmp
	adc o3, tmp
	sp_ld ,, tmp
	adc o4, tmp
	sp_ld ,, tmp
	adc o5, tmp
	sp_ld ,, tmp
	adc o6, tmp
	sp_ld ,, tmp
	adc o7, tmp
.endm

.macro sub64_stack tmp:req, o0:req, o1:req, o2:req, o3:req, o4:req, o5:req, o6:req, o7:req
	sp_ld ,, tmp
	sub o0, tmp
	sp_ld ,, tmp
	sbc o1, tmp
	sp_ld ,, tmp
	sbc o2, tmp
	sp_ld ,, tmp
	sbc o3, tmp
	sp_ld ,, tmp
	sbc o4, tmp
	sp_ld ,, tmp
	sbc o5, tmp
	sp_ld ,, tmp
	sbc o6, tmp
	sp_ld ,, tmp
	sbc o7, tmp
.endm

.macro add64_8 o0:req, o1:req, o2:req, o3:req, o4:req, o5:req, o6:req, o7:req, in:req
	add o0, in
	clr in
	adc o1, in
	adc o2, in
	adc o3, in
	adc o4, in
	adc o5, in
	adc o6, in
	adc o7, in
.endm

.macro sub64_8 o0:req, o1:req, o2:req, o3:req, o4:req, o5:req, o6:req, o7:req, in:req
	sub o0, in
	clr in
	sbc o1, in
	sbc o2, in
	sbc o3, in
	sbc o4, in
	sbc o5, in
	sbc o6, in
	sbc o7, in
.endm

.macro rol64 o0:req, o1:req, o2:req, o3:req, o4:req, o5:req, o6:req, o7:req
	bst o7, 7
	lsl o0
	rol o1
	rol o2
	rol o3
	rol o4
	rol o5
	rol o6
	rol o7
	bld o0, 0
.endm

.macro ror64 o0:req, o1:req, o2:req, o3:req, o4:req, o5:req, o6:req, o7:req
	bst o0, 0
	lsr o7
	ror o6
	ror o5
	ror o4
	ror o3
	ror o2
	ror o1
	ror o0
	bld o7, 7
.endm

.macro eor64 o0:req, o1:req, o2:req, o3:req, o4:req, o5:req, o6:req, o7:req, i0:req, i1:req, i2:req, i3:req, i4:req, i5:req, i6:req, i7:req
	eor o0, i0
	eor o1, i1
	eor o2, i2
	eor o3, i3
	eor o4, i4
	eor o5, i5
	eor o6, i6
	eor o7, i7
.endm

; rotate left by variable number of bits, amount may be negative
.macro rot64 amount:req, regs:vararg
	.if amount>0
		.rept amount
			rol64 regs
		.endr
	.elseif amount<0
		.rept -(amount)
			ror64 regs
		.endr
	.endif
.endm

.noaltmacro
