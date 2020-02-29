#!/bin/bfmake

stk 16
org 0
txt "Hello"

psh 5
psh 6
psh 7
psh 8
psh 9
psh 10
psh 11
psh 12
psh 13
psh 14
psh 15

pop r2
pop r3
mod r2, r3
pop r3
add r2, r3
pop r4
add r2, r4
pop r3
add r2, r3
pop r3
add r2, r3
pop r3
add r2, r3
pop r3
add r2, r3
pop r3
add r2, r3
pop r3
add r2, r3
mod r2, 20
sub r2, 6
add r2, .0
out r2 ; should display 6

rcl r1, 0
out r1
rcl r1, 1
out r1
rcl r1, 2
out r1
rcl r1, 3
out r1
rcl r1, 4
out r1
; will display Hello
