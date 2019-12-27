#!/bin/bfmake

stk 2
org 0
seg 0

txt "Quick brown fox jumps over the lazy dog."

; (48 << 1 >> 1) = 48 * 2 / 2 = 48
bpl .0
stl
str
lev r1
out r1

; ((0 + 25) * 4 / 2) - 1 = 49
bpl 0
sta 25
stm 4
std 2
sts 1
lev r1
out r1

; 8^2+1 = 65
bpl 8
stp 2
sta 1
lev r1
out r1

; (3 mod 2) + 48 = 49
bpl 3
stq 2
sta .0
lev r1
out r1

; rcl r1, 8 = 'o'
bpl 8
lea r1
out r1

