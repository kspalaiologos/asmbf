
; calculate 2/7 * 5/3

psh 7
psh 2
psh 5
psh 3
fmul
freduce
pop r1
add r1, .0
out r1
pop r1
out r1
