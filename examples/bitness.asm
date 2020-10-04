#!/bin/asmbf/bfmake

; An interpreter bitness check.

stk 3
org 0
seg 0

psh %return
jmp %bitness
@return
    out r1
    end

; label-less bitness test.
; r1 <= interpreter's bitness
@bitness
    psh r2
    clr r1
    mov r2, 1
    nav r2
    raw .[
    inc r1
    asl r2
    nav r2
    raw .]
    pop r2
    ret
