#!/bin/bfmake

mov r1, __COUNTER__          ; 0
mov r2, __COUNTER__          ; 1
mov r3, __COUNTER__          ; 2
mov r4, __COUNTER__          ; 3

add r1, .0
add r2, .0
add r3, .0
add r4, .0

out r1
out r2
out r3
out r4
