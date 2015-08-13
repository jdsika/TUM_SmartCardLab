; AVR 8-bit microcontroller stack pointer repurposing
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


; Overview
; ========
;
; These routines abuse the stack pointer as a generic pointer register
; to a user-defined structure. Your struct must be less than 256 bytes
; long.
;
; The current position is kept track of at assemble time by using
; symbols, so your access patterns must be static.
;
; It is your responsibiliy to save the true stack pointer before using
; these macros, and to restore it afterwards. Also, you have to set SPH
; manually.
;
; By using the API below, cycles wasted due to pointer calculations
; will be minimized.
;
; API
; ===
;
; sp_init <pos>
;   Setup the internal symbols for a stack pointer that is currently
;   pointing at position <pos> inside your structure
;
; sp_reg <posreg>
;  Prepare a temporary position register as needed by sp_ld/sp_st.
;  <posreg> must be from r16 - r31
;
; sp_ld [ <posreg> ], [ <pos> ], <reg> ...
;   Load a sequence of bytes in the exact order given.
;   If <posreg> and <pos> are set, then the first register will be
;   loaded from <pos>.
;   If <pos> is blank, the first register is loaded from the current
;   position.
;   After loading, the current position points to the byte after
;   the stored values.
;   <posreg> is allowed to appear in <reg> as well.
;
; sp_st [ <posreg> ], [ <pos> ], <reg> ...
;   Store a sequence of bytes in the exact order given.
;   If <posreg> and <pos> are set, then the first register will be
;   stored at <pos>.
;   If <pos> is blank, the last byte will be stored below the
;   current position.
;   After storage, the current position points to the first byte.
;
; sp_goto <posreg>, <pos>
;   manually move the current position for later use by the
;   reduced-parameter variants of sp_ld/sp_st

.altmacro

; should be the same everywhere
stack_hi = 0x3e ; SPH
stack_lo = 0x3d ; SPL

; Set the expected data alignment. The value 256 will be most efficient.
sp_align = 256

; these symbols keep track of where we are currently pointing at
.Lsp_goto_pos = 1
.Lsp_goto_stackpos = 1

.macro sp_init pos:req
	.Lsp_goto_pos = pos + 1
	.Lsp_goto_stackpos = pos + 1
.endm

.macro sp_reg reg:req
	.if sp_align != 256
		in reg, stack_lo
		.Lsp_goto_pos = .Lsp_goto_stackpos
	.else
		.Lsp_goto_pos = -1
	.endif
.endm

.macro sp_goto reg:req, to:req
	.if .Lsp_goto_stackpos != to
		.if .Lsp_goto_pos != to
			.if sp_align != 256
				subi reg, (.Lsp_goto_pos)-(to)
			.else
				ldi reg, (to)-1
			.endif
			.Lsp_goto_pos = to
		.endif
		out stack_lo, reg
		.Lsp_goto_stackpos = to
	.endif
.endm

.macro sp_st posreg = X, pos = X, regs:vararg
	LOCAL cnt, curcnt, curpos
    cnt = 0
	.irp reg, regs
		cnt = cnt + 1
	.endr
	.ifnc pos, X
		sp_goto posreg, pos+cnt
	.endif
	curcnt = cnt
	.rept curcnt
		curcnt = curcnt - 1
		curpos = 0
		.irp reg, regs
			.if curpos == curcnt
				push reg
			.endif
			curpos = curpos + 1
		.endr
	.endr
	.Lsp_goto_stackpos = .Lsp_goto_stackpos - cnt
.endm

.macro sp_ld posreg = X, pos = X, regs:vararg
	.ifnc pos, X
		sp_goto posreg, pos
	.endif
	.irp reg, regs
		pop reg
		.Lsp_goto_stackpos = .Lsp_goto_stackpos + 1
	.endr
.endm

.noaltmacro
