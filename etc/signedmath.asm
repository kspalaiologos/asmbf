
; Note: This file is some sort of a scratchpad for me.
; The contents will change.

; 7) 5 - 3
mov r1, $(signed(5))
mov r2, $(signed(3))
#call("sign_sub")
mov r2, $(signed(2))
eq r1, r2
add r1, .0
out r1

; 8) 3 - 5
mov r1, $(signed(3))
mov r2, $(signed(5))
#call("sign_sub")
mov r2, $(signed(-2))
eq r1, r2
add r1, .0
out r1

; 9) -5 - 3
mov r1, $(signed(-5))
mov r2, $(signed(3))
#call("sign_sub")
mov r2, $(signed(-8))
eq r1, r2
add r1, .0
out r1

; 10) -3 - 5
mov r1, $(signed(-3))
mov r2, $(signed(5))
#call("sign_sub")
mov r2, $(signed(-8))
eq r1, r2
add r1, .0
out r1

; 11) 10 - -3
mov r1, $(signed(10))
mov r2, $(signed(-3))
#call("sign_sub")
mov r2, $(signed(13))
eq r1, r2
add r1, .0
out r1

; 12) 3 - -10
mov r1, $(signed(3))
mov r2, $(signed(-10))
#call("sign_sub")
mov r2, $(signed(13))
eq r1, r2
add r1, .0
out r1

; 13) -1 - -5
mov r1, $(signed(-1))
mov r2, $(signed(-5))
#call("sign_sub")
mov r2, $(signed(4))
eq r1, r2
add r1, .0
out r1

; 14) -5 - -1
mov r1, $(signed(-5))
mov r2, $(signed(-1))
#call("sign_sub")
mov r2, $(signed(-4))
eq r1, r2
add r1, .0
out r1

; 15) -1 + -5
mov r1, $(signed(-1))
mov r2, $(signed(-5))
#call("sign_add")
mov r2, $(signed(-6))
eq r1, r2
add r1, .0
out r1


end

; -----------------------------------------------------
; sign_add: Add r2 to r1.
; r1 += r2
; Modified: r1
; Trashed: r2
@sign_add
    ; Check the sign.
    push r2
    mod r2, 2
    ceq r2, 1
    pop r2
    ; conditional flag is set if the number is negative.
    ; make it negative.
    inc r2
    ; uh oh! the last bits are 10, so make it 0.
    csub r2, 2
    jmp %sign_sub

; -----------------------------------------------------
; sign_div: Divide r2 with & to r1.
; r1 /= r2
; Modified: r1
; Trashed: r2
@sign_div
    mov f3, 1
    jmp %sign_opstub

; -----------------------------------------------------
; sign_mul: Multiply r2 with & to r1.
; r1 *= r2
; Modified: r1
; Trashed: r2
@sign_mul
    mov f3, 0
    jmp %sign_opstub

; -----------------------------------------------------
; sign_mod: Calculate r1 % r2, put the result in r1
; r1 %= r2
; Modified: r1
; Trashed: r2
@sign_mod
    mov f3, 2
    jmp %sign_opstub

; -----------------------------------------------------
; sign_opstub: internal use only.
@sign_opstub
    push r3
    push r4
    
    mov r3, r1
    mov r4, r2

    mod r3, 2
    mod r4, 2

    asr r1
    asr r2

    ceq f3, 0
    cmul r1, r2
    ceq f3, 1
    cdiv r1, r2
    ceq f3, 2
    cmod r1, r2

    asl r1

    neq r3, r4
    add r1, r3

    pop r4
    pop r3
    ret

; -----------------------------------------------------
; sign_sub: Subtract r1 from r2.
; r1 -= r2
; Modified: r1
; Trashed: r2
@sign_sub
    ; r3 = sign of r1
    ; r4 = sign of r2
    mov f3, r1
    mov f2, r2
    mod f3, 2
    mod f2, 2

    ; Remove the sign bit.
    asr r1
    asr r2

    ; x>0 ^ y>0
    ceq f3, 0
    candeq f2, 0
    fps
    candgt r1, r2
    csub r1, r2
    casl r1
    fpo
    candle r1, r2
    cxchg r1, r2
    csub r1, r2
    casl r1
    cadd r1, 1

    ; x>0 ^ y<0
    ceq f3, 0
    candeq f2, 1
    cadd r1, r2
    casl r1

    ; x<0 ^ y>0
    ceq f3, 1
    candeq f2, 0
    cadd r1, r2
    casl r1
    cadd r1, 1

    ; x<0 ^ y<0
    ceq f3, 1
    candeq f2, 1
    fps
    candge r1, r2
    csub r1, r2
    casl r1
    cadd r1, 1
    fpo
    candlt r1, r2
    cxchg r1, r2
    csub r1, r2
    casl r1

    ceq r1, 1
    cmov r1, 0

    ret
