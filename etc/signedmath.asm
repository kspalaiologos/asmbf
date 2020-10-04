
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
add r1, .0
out r1

; 4) signum on x < 0
mov r1, $(signed(-5))
#call("signum")
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

mov r1, $(signed(5))
mov r2, $(signed(3))
#call("sign_sub")
mov r2, $(signed(2))
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
; sign_sub: Subtract r1 from r2.
; r1 -= r2
; Modified: r1
; Trashed: r2
@sign_sub
    push r3
    push r4

    ; r3 = sign of r1
    ; r4 = sign of r2
    mov r3, r1
    mov r4, r2
    mod r3, 2
    mod r4, 2

    ; Remove the sign bit.
    asr r1
    asr r2

    ; x>0 ^ y>0
    ceq r3, 1
    candeq r4, 1
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
    ceq r3, 1
    candeq r4, 0
    cadd r1, r2

    ; x<0 ^ y>0
    ceq r3, 0
    candeq r4, 1
    cadd r1, r2
    casl r1
    cadd r1, 1

    ; x<0 ^ y<0
    ceq r3, 0
    candeq r4, 0
    fps
    candge r1, r2
    csub r1, r2
    cadd r1, 1
    fpo
    candlt r1, r2
    cxchg r1, r2
    csub r1, r2

    pop r4
    pop r3
    ret
