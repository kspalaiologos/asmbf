#!/bin/asmbf/bfmake
; Build me by executing!

org 0
stk 0

mov r4, .F

mov r1, .0
mov r2, r1

lbl 1
	out r1
	out r2
	out 32
	
	mov r3, r1
	eq_ r3, .9
	jnz r3, 2
	
	mov r3, r2
	eq_ r3, .9
	jnz r3, 3
	
	mov r3, r1
	eq_ r3, r4
	jnz r3, 5
	
lbl 6
	mov r3, r2
	eq_ r3, r4
	jnz r3, 4
	
	inc r2
	
	jmp 1
lbl 2
	sub r4, 5
	mov r1, r4
	add r4, 5
	jmp 1
lbl 3
	sub r4, 5
	mov r2, r4
	add r4, 5
	jmp 1
lbl 4
	inc r1
	mov r2, .0
	jmp 1
lbl 5
	mov r3, r2
	eq_ r3, r4
	jz_ r3, 6
