; have tparam be seen by everyone
.global tparam_asm
; mach im datensegment weiter
;.data
; pad location pointer to align by 256 bytes
.balign 256
; definiere tparam und speicher 72 0en dort
tparam_asm: .fill 72, 0
; mach im programmcode weiter
;.text
