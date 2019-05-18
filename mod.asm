
; Modulus polyfill because bfasm's got it broken

; r1 - n & r2 - x => r2 = n mod x
:mod
    psh r3
    mov r3, r2
    div r3, r1
    sub r2, r3
    pop r3
    ret
