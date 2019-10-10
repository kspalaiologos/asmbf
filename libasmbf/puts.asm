
; puts(r1) - print out a null terminated string.
; r1 will point to the end of the string.

; Will print a newline at the end if fputs
; isn't defined.

; PSSR: 2 slots minimum, MR = n r/0 w.
; Registers preserved.
; Label complexity: 3
; Constants: 0/10 bytes (fputs)
; WCBS: n+1 branches
; BS is constant.
; IA: 12 instructions

#if 0

This file is a part of universal asm2bf runtime.
Copyright (C) by Krzysztof Palaiologos Szewczyk, 2019.

#endif

#ifndef _LIBASMBF_PUTS
#define _LIBASMBF_PUTS

@puts
    psh r2
@puts_loop
	rcl r2, r1
	jz_ r2, %puts_end
	out r2
	inc r1
	jmp %puts_loop
@puts_end
	#ifndef fputs
	out 10
	#endif
	pop r2
	ret

#endif
