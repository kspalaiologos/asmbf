
#ifndef __AND
#define __AND

; a = r1, b = r2
; result = r2
; r3, r4 preserved.

@and
	psh r1
	psh r4
	psh r3
	mov r4, 16
	pow r4, 4
	asr r4
	clr r3
@andloop
	psh r1
	psh r2
	ge_ r1, r4
	ge_ r2, r4
	and r1, r2
	jnz r1, %andcase1
	pop r2
	pop r1
	psh r1
	psh r2
	ge_ r1, r4
	ge_ r2, r4
	jnz r1, %andcase2
	jnz r2, %andcase3
	pop r2
	pop r1
@andfinish
	asr r4
	jnz r4, %andquit
	mov r2, r3
	pop r3
	pop r4
	pop r1
	ret
@andcase1
	add r3, r4
	jmp %andfinish
@andcase2
	sub r1, r4
	jmp %andfinish
@andcase3
	sub r2, r4
	jmp %andfinish

#endif
