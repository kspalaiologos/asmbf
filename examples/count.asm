
; A hexadecimal counter, written in pure bfasm.
; You can build it without preprocessing.

; Note: That's not a perfect example of how asm2bf looks nowadays.
; If you want to help making it better, please send a pull request.

org 0
stk 0

mov r4, .F

mov r1, .0
mov r2, r1

lbl 1
    out r1
    out r2
    out 32
    
    mov r3, r1
    eq r3, .9
    jnz r3, 2
    
    mov r3, r2
    eq r3, .9
    jnz r3, 3
    
    mov r3, r1
    eq r3, r4
    jnz r3, 5
    
lbl 6
    mov r3, r2
    eq r3, r4
    jnz r3, 4
    
    inc r2
    
    jmp 1
lbl 2
    sub r4, 5
    mov r1, r4
    add r4, 5
    jmp 1
lbl 3
    sub r4, 5
    mov r2, r4
    add r4, 5
    jmp 1
lbl 4
    inc r1
    mov r2, .0
    jmp 1
lbl 5
    mov r3, r2
    eq r3, r4
    jz r3, 6
