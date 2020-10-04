
; Complex test suite testing remainants of asm2bf.
; we can skip:
;  -> amp
;  -> smp
;  -> rcl
;  -> out
;  -> sto
;  -> db_
;  -> nav
;  -> seg
;  -> txt
;  -> jz_

stk 8
org 0
db 8

in r1        ; r1 = 65
add r1, 13    ; r1 = 78
mov r2, r1    ; r1 = 78, r2 = 78
add r1, r2    ; r1 = 156, r2 = 78
mov r3, r1    ; r1 = 156, r2 = 78, r3 = 156, r4 = 0
log r3        ; r1 = 156, r2 = 78, r3 = 1, r4 = 0
and r3, r4    ; r1 = 156, r2 = 78, r3 = 0, r4 = 0
add r3, .0    ; r1 = 156, r2 = 78, r3 = 48, r4 = 0
out r3        ; r1 = 156, r2 = 78, r3 = 48, r4 = 0
asr r1        ; r1 = 78, r2 = 78, r3 = 48, r4 = 0
out r1        ; r1 = 78, r2 = 78, r3 = 48, r4 = 0
asl r1        ; r1 = 156, r2 = 78, r3 = 48, r4 = 0
out r1        ; r1 = 156, r2 = 78, r3 = 48, r4 = 0
clr r1        ; r1 = 0, r2 = 78, r3 = 48, r4 = 0
out r1        ; r1 = 0, r2 = 78, r3 = 48, r4 = 0
inc r3        ; r1 = 0, r2 = 78, r3 = 49, r4 = 0
out r3        ; r1 = 0, r2 = 78, r3 = 49, r4 = 0
dec r3        ; r1 = 0, r2 = 78, r3 = 48, r4 = 0
out r3        ; r1 = 0, r2 = 78, r3 = 48, r4 = 0
div r3, 4     ; r1 = 0, r2 = 78, r3 = 12, r4 = 0
mov r4, r3    ; r1 = 0, r2 = 78, r3 = 12, r4 = 12
eq r4, r3    ; r1 = 0, r2 = 78, r3 = 12, r4 = 1
ge r3, r4    ; r1 = 0, r2 = 78, r3 = 0, r4 = 1
add r3, r4    ; r1 = 0, r2 = 78, r3 = 1, r4 = 1
out r3        ; r1 = 0, r2 = 78, r3 = 1, r4 = 1
mov r3, %l    ; r1 = 0, r2 = 78, r3 = 1, r4 = 1
jmp r3
out .!

@l
    jnz r1, 0
    mod r2, 2 ; r1 = 0, r2 = 0, r3 = 1, r4 = 1
    out r2
    inc r3    ; r1 = 0, r2 = 0, r3 = 2 r4 = 1
    mul r3, 8 ; r1 = 0, r2 = 0, r3 = 16, r4 = 1
    out r3
    neg r3
    out r3
    log r3
    not r3
    out r3
    psh r3
    pop r3
    out r3
    psh 9
    pop r3
    out r3
    psh 5
    psh 9
    srv
    pop r4
    out r4
    pop r3
    swp r4, r3
    out r3
    psh 0
    ret
