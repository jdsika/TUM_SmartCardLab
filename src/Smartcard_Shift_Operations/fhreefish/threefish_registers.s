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

.altmacro

; To understand this, you should be familiar with the Skein specification.
; In these macros, words are referred to like "w0j0", meaning word 0
; of j-value 0 (j-value ~= MIX operation j+1). Bytes within words are
; addressed like "b0w0j0".
; Skein uses linear numbering. Skein word number = w + j*2



.macro _regset reg:req, word:req, jval:req, val:req
	.Lb&reg&w&word&j&jval = val
.endm
; Initialize automatic register mapping.

; Current register layout is stored in symbols .Lb<byte>w<word>j<j-value>.
; Actual register numbers are completely transparent to the outside, see
; regcall below.
; start layout: r0-r15 == j-value 1, r16-r31 == j-value 0
; r16-r31 support more operations than r0-r15. Since w0j0 is used less
; during add_subkey (no tweak/round addition), we place j0 in r16-r31,
; which reduces register swapping.
.macro reginit
	.irp word, 0, 1
		.irp jval, 0, 1
			.irp reg, 0, 1, 2, 3, 4, 5, 6, 7
				_regset reg, word, jval, 16-16*jval + word*8 + reg
			.endr
		.endr
	.endr
.endm


.macro _regrotate1 reg:req, word:req, jval:req, bytes:req
	LOCAL offset
	offset = (.Lb&reg&w&word&j&jval / 8) * 8
	.Lb&reg&w&word&j&jval =  (((.Lb&reg&w&word&j&jval - offset) + 8 - bytes) % 8) + offset
.endm
; Rotate words by renaming registers plus small amounts of bit rotation

; prefer_right influences rotation in case bits%8 == 4:
;   if unset or 0, it will be a (bits%8) byte rotation and 4 bit shifts left
;   if 1, it will be a (bits%8)+1 byte rotation and 4 bit shifts right
; Since byte rotation is done via register renaming, this fine-tunes the final
; register layout.
.macro rotate word:req, jval:req, bits:req, prefer_right
	LOCAL bias
	bias = prefer_right+3
	.irp reg, 0, 1, 2, 3, 4, 5, 6, 7
		_regrotate1 reg, word, jval, (bits+bias)/8
	.endr
	regcall rot64, %((bits+bias)%8)-bias, w&word&j&jval
.endm


; setup temporary symbols for word permutation
.macro regtmpset reg:req, word:req, jval:req
	.Ltmp&reg&w&word&j&jval = .Lb&reg&w&word&j&jval
.endm
.macro regtmpinit
	.irp word, 0, 1
		.irp jval, 0, 1
			.irp reg, 0, 1, 2, 3, 4, 5, 6, 7
				regtmpset reg, word, jval
			.endr
		.endr
	.endr
.endm

.macro permute1 reg:req, w:req, j:req, perm:req
	.if perm == 0
		_regset reg, w, j, .Ltmp&reg&w0j0
	.elseif perm == 1
		_regset reg, w, j, .Ltmp&reg&w1j0
	.elseif perm == 2
		_regset reg, w, j, .Ltmp&reg&w0j1
	.elseif perm == 3
		_regset reg, w, j, .Ltmp&reg&w1j1
	.else
		.err "invalid perm value"
	.endif
.endm
; Permute words by register renaming
; w0-3 are linear word numbers as used in the Skein specification
.macro permute w0:req, w1:req, w2:req, w3:req
	regtmpinit
	.irp reg, 0, 1, 2, 3, 4, 5, 6, 7
		permute1 reg, 0, 0, w0
		permute1 reg, 1, 0, w1
		permute1 reg, 0, 1, w2
		permute1 reg, 1, 1, w3
	.endr
.endm

.macro _regcall name:req, narg, args:vararg
	.ifnb narg
		.ifc narg, w0j0
			_regcall name, args, %.Lb0w0j0, %.Lb1w0j0, %.Lb2w0j0, %.Lb3w0j0, %.Lb4w0j0, %.Lb5w0j0, %.Lb6w0j0, %.Lb7w0j0
		.else
		.ifc narg, w1j0
			_regcall name, args, %.Lb0w1j0, %.Lb1w1j0, %.Lb2w1j0, %.Lb3w1j0, %.Lb4w1j0, %.Lb5w1j0, %.Lb6w1j0, %.Lb7w1j0
		.else
		.ifc narg, w0j1
			_regcall name, args, %.Lb0w0j1, %.Lb1w0j1, %.Lb2w0j1, %.Lb3w0j1, %.Lb4w0j1, %.Lb5w0j1, %.Lb6w0j1, %.Lb7w0j1
		.else
		.ifc narg, w1j1
			_regcall name, args, %.Lb0w1j1, %.Lb1w1j1, %.Lb2w1j1, %.Lb3w1j1, %.Lb4w1j1, %.Lb5w1j1, %.Lb6w1j1, %.Lb7w1j1
		.else
			_regcall name, args, narg
		.endif
		.endif
		.endif
		.endif
	.else
		name args
	.endif
.endm
; Call some other macro with current register mapping

; args are passed verbatim except for those that look like "w0j0". These
; are replaced with 8 args listing the current register mapping for that
; argument.
.macro regcall name:req, args:vararg
	_regcall name, args,
.endm

.noaltmacro
