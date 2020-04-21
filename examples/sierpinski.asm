
; Sierpinski triangle fractal viewer for asm2bf obfuscated code contest.
; Released to the public domain by Palaiologos/MENACE @ Apr 2020

@a
clrr2
@b
pshr1
movr4,r2
@c
movr5,r1
modr5,2
movr6,r4
modr6,2
mulr5,r6
cger5,1
cmor5,1
cjn%d
asrr1
asrr4
jnzr1,%c
jnzr4,%c
clrr3
@d
cger3,1
movr3,.*
cmor3,. 
outr3
popr1
incr2
cger2,64
cjz%b
out10
incr1
cger1,64
cjz%a
