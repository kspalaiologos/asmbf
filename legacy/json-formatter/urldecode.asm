
; Remove blocks between
; -----------
; and
; -----------
; If you don't need parameter amount checking code.
; (you probably don't)

stk 0
org 0

mov r4, .0

lbl 1
	in_ r1
	jz_ r1, 0
	mov r2, r1
	eq_ r2, .%
	jnz r2, 2
	; -----------
	mov r2, r1
	eq_ r2, .&
	jnz r2, 0
	; -----------
	mov r2, r1
	eq_ r2, .+
	jnz r2, 7
	out r1
	jmp 1
lbl 2
	in_ r1
	in_ r2
	mov r3, r1
	ge_ r3, .A
	jnz r3, 3
	jmp 4
lbl 3
	sub r1, 7
lbl 4
	sub r1, r4
	mov r3, r2
	ge_ r3, .A
	jnz r3, 5
	jmp 6
lbl 5
	sub r2, 7
lbl 6
	sub r2, r4
	mul r1, 16
	add r1, r2
	out r1
	jmp 1
lbl 7
	out 32
	jmp 1
