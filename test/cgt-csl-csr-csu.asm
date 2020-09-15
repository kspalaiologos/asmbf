#!/bin/asmbf/bfmake
mov r1, 2 ; r1 = 2
cgt r1, 1 ; true
csl r1    ; 2 * 2
csl r1    ; 2 * 2 * 2
csr r1    ; 2 * 2
csu r1, 1 ; 2 * 2 - 1
cad r1, 48; 2 * 2 + 47
out r1    ; 4 + 47 = '3'