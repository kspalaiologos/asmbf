
; calculate 1/n for n in 1 .. 10

inc r1
psh r1
psh 1

@loop
	inc r1
	psh r1
	psh 1
	raw .1
	fadd
	freduce
	raw .1
	ceq r1, 9
	cjn %q
	jmp %loop
@q
	pop r1
	pop r2
	eq  r1, 7129
	eq  r2, 2520
	and r1, r2
	add r1, .0
	out r1
