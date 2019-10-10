
; r1 = isspace(r1)
; r1 = (r1 > 8 & r1 < 14) | r1 = 32

; PSSR: 4 slots minimum, MR = 0.
; Registers preserved.
; Label complexity: 2
; Constants: 54 bytes
; WCBS: 2 branches
; BCBS: 1 branch
; IA: 21 instructions

#if 0

This file is a part of universal asm2bf runtime.
Copyright (C) by Krzysztof Palaiologos Szewczyk, 2019.

#endif

#ifndef _LIBASMBF_ISSPACE
#define _LIBASMBF_ISSPACE

@isspace
	psh r2
	
	mov r2, r1
	eq_ r2, 32
	jnz r2, %isspace_true
	
	mov r2, r1
	gt_ r2, 8
	psh r3
	psh r2
	
	mov r2, r1
	lt_ r2, 14
	pop r3
	and r2, r3
	pop r3
	
	jnz r2, %isspace_true
	
	clr r1
	pop r2
	ret

@isspace_true
	mov r1, 1
	pop r2
	ret

#endif
