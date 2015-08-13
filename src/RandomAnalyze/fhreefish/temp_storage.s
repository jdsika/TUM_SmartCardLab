; AVR 8-bit microcontroller 64 temporary storage helpers
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

;
; Temporary Storage Helpers
; =========================
;
; These helpers auto-optimize global variable access.
; If the address is below 0x20 (register file), MOV is used.
; If the address is below 0x60 IN/OUT are used.
; Otherwise, LDS is used (2 cycles instead of 1).
;
; temp_st <addr>, <reg>
; temp_ld <reg>, <addr>
;
; This only works if <addr> is constant.
;

.altmacro

.macro temp_st addr:req, reg:req
	.if addr < 0x20
		mov addr, reg
	.elseif addr < 0x60
		out addr-0x20, reg
	.else
		sts addr, reg
	.endif
.endm

.macro temp_ld reg:req, addr:req
	.if addr < 0x20
		mov reg, addr
	.elseif addr < 0x60
		in reg, addr-0x20
	.else
		lds reg, addr
	.endif
.endm


.noaltmacro
