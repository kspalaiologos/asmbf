
@r
	in r1
	jz r1, 0
	inc r2
	@l
	mov r3,r1
	mod r3,r2
	ceq r3,0
	cad r1,1
	cad r2,1
	cjn %l
	dec r2
	out r2
	jmp %r