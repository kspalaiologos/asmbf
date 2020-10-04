
$(
    function signed(x)
        if x <= 0 then
            return (-x) * 2 + 1
        else
            return x * 2
        end
    end
)

; 1) abs on x >= 0.
mov r1, $(signed(5))
mov r2, r1

#call("abs")
eq r1, r2
add r1, .0
out r1

; 2) abs on x < 0
mov r1, $(signed(-5))
mov r2, $(signed(5))

#call("abs")
eq r1, r2
add r1, .0
out r1

; 3) signum on x > 0
mov r1, $(signed(5))
#call("signum")
eq r1, 0
add r1, .0
out r1

; 4) signum on x < 0
mov r1, $(signed(-5))
#call("signum")
eq r1, 1
add r1, .0
out r1

; 5) negate on x < 0
mov r1, $(signed(-5))
mov r2, $(signed(5))
#call("negate")
eq r1, r2
add r1, .0
out r1

; 6) negate on x > 0
mov r1, $(signed(5))
mov r2, $(signed(-5))
#call("negate")
eq r1, r2
add r1, .0
out r1

; Note: -0 == +0 only after normalization.

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
; signum: Return +0 for -0, and n for any other number.
; r1 = 1 => r1 was negative
; r1 = 0 => r1 was positive
; Modified: r1
@normalize
    ceq r1, 1
    csub r1, 1
    ret

; -----------------------------------------------------
; signum: Return the sign of a number in r1.
; r1 = 1 => r1 was negative
; r1 = 0 => r1 was positive
; Modified: r1
@signum
    mod r1, 2
    ret


; -----------------------------------------------------
; abs: Return the absolute value of a number in r1.
; r1 >= 0: no change
; r1 < 0: r1 = -r1
; Modified: r1
@abs
    asr r1
    asl r1
    ret

; -----------------------------------------------------
; negate: Negate the number in r1.
; r1 = -r1
; Modified: r1
@negate
    push r1
    mod r1, 2
    ceq r1, 1
    pop r1
    inc r1
    csub r1, 2
    ceq r1, 1
    csub r1, 1
    ret

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
; sign_mul: Multiply r2 with & to r1.
; r1 *= r2
; Modified: r1
; Trashed: r2
@sign_mul
    push r3
    push r4
    
    mov r3, r1
    mov r4, r2

    mod r3, 2
    mod r4, 2

    asr r1
    asr r2

    mul r1, r2
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

    ret
