#!/bin/asmbf/bfmake

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

pop r5
pop r6
mod r5, r6
pop r6
add r5, r6
pop r6
add r5, r6
pop r6
add r5, r6
pop r6
add r5, r6
pop r6
add r5, r6
pop r6
add r5, r6
pop r6
add r5, r6
pop r6
add r5, r6
pop r6
add r5, r6
mod r5, 20
sub r5, 2
add r5, .0
out r5 ; should display 0

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
