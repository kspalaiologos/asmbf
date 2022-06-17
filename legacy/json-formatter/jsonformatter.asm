
stk 10
org 10

txt "Content-Type: application/json"
db_ 10
db_ 0

mov r1, 10

lbl 15
	rcl r2, r1
	jz_ r2, 16
	out r2
	inc r1
	jmp 15
lbl 16
	out 10

clr r1
clr r2

lbl 17
	in_ r1
	eq_ r1, .=
	jz_ r1, 17

psh 0
lbl 1
	in_ r1
	jz_ r1, 0
	mov r3, r1
	eq_ r3, 13
	jnz r3, 1
	mov r3, r1
	eq_ r3, 10
	jnz r3, 1
	mov r3, r1
	eq_ r3, 58
	jnz r3, 6
	mov r3, r1
	eq_ r3, 32
	jnz r3, 4
	mov r3, r1
	eq_ r3, 34
	jnz r3, 5
	mov r3, r1
	eq_ r3, .\
	jnz r3, 7
	mov r3, r1
	eq_ r3, .,
	jnz r3, 8
	mov r3, r1
	eq_ r3, .]
	psh r4
	mov r4, r1
	eq_ r4, .}
	or_ r3, r4
	pop r4
	jnz r3, 9
	mov r3, r1
	eq_ r3, .{
	psh r4
	mov r4, r1
	eq_ r4, .[
	or_ r3, r4
	pop r4
	jnz r3, 14
	pop r3
	jz_ r3, 10
	clr r3
lbl 10
	psh r3
	out r1
	jmp 1
lbl 2
	clr r3
lbl 11
	psh r3
	ge_ r3, r2
	jnz r3, 12
	out 9
	pop r3
	inc r3
	jmp 11
lbl 12
	pop r3
	ret
lbl 6
	out r1
	jnz r4, 1
	out 32
	jmp 1
lbl 4
	jz_ r4, 1
	jmp 3
lbl 5
	pop r3
	psh r3
	jnz r3, 3
	not r4
lbl 3
	out r1
	jmp 1
lbl 7
	pop r3
	not r3
	psh r3
	out r1
	jmp 1
lbl 8
	out r1
	jnz r4, 1
	out 10
	psh 1
	jmp 2
lbl 9
	jnz r4, 3
	jz_ r2, 13
	dec r2
lbl 13
	out 10
	psh 3
	jmp 2
lbl 14
	out r1
	jnz r4, 1
	inc r2
	out 10
	psh 1
	jmp 2