
; Modulus polyfill because bfasm's got it broken

; r1 - n & r2 - x => r2 = n mod x
:mod
    psh r3
    mov r3, r2
    div r3, r1
    sub r2, r3
    pop r3
    ret

; PTR stack[--sp] -> stdout
:puts
    pop r1
:puts_1
    rcl r2,r1
    jz_ r2,[puts_2]
    out r2
    inc r1
    jmp [puts_1]
:puts_2
    out 10
    ret
