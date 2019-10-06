#!/bin/bfmake

stk 2
org 0

; Build brainfuck file by executing me!

#include <example-lib.asm>

psh 2
jmp print_zero
lbl 2
	end
