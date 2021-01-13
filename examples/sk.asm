
; SK calculus evaluator.
; Doesn't garbage collect, has very limited recursion depth.
; you can tweak this limit by changing the `stk` parameter below.
; Copyright (C) Kamila Szewczyk, 2021.

[bits 8]

stk 38
org 0

&heap
db 2

&changed
db 0

#call("read")
@loop
#call("eval")
    rcl r2, *changed
    vxcall sto *changed, 0
    jnz r2, %loop
    push 0
@P
    push r2
    add r1, 2
    rcl r2, r1
    sub r1, 2
    jz r2, %PS
    dec r2
    jz r2, %PK
    out .(
    mov r2, r1
    rcl r1, r2
#call("P")
    inc r2
    rcl r1, r2
#call("P")
    out .)
    pop r2
    ret
@PS
    out .S
    pop r2
    ret
@PK
    out .K
    pop r2
    ret
@A
    rcl f3, *heap
    add f3, 2
    sto f3, r1
    inc f3
    ots f3, *heap
    sub f3, 3
    mov r1, f3
    ret
@read
    push r2
    in r2
    ceq r2, .S
    cjnz %RS
    ceq r2, .K
    cjnz %RK
    mov r1, 2
#call("A")
    mov r2, r1
#call("read")
    sto r2, r1
#call("read")
    inc r2
    sto r2, r1
    dec r2
    mov r1, r2
    in r2
    pop r2
    ret
@RS
    mov r1, 0
#call("A")
    pop r2
    ret
@RK
    mov r1, 1
#call("A")
    pop r2
    ret
@eval
    push r2
    push r3
    rcl r2, r1
    jz r2, %skip
    rcl r3, r2
    jz r3, %skip
    add r3, 2
    rcl r4, r3
    cne r4, 1
    cjnz %skip
    inc r2
    rcl r3, r2
    mov r1, r3
    vxcall sto *changed, 1
    jmp %not_bi
@skip
    rcl r2, r1
    jz r2, %notS
    rcl r3, r2
    jz r3, %notS
    rcl r4, r3
    jz r4, %notS
    add r4, 2
    rcl r2, r4
    jnz r2, %notS
    inc r1
    rcl r6, r1
    dec r1
    rcl r2, r1
    rcl r3, r2
    inc r2
    inc r3
    rcl r5, r2
    rcl r4, r3
    mov r1, 2
#call("A")
    mov r2, r1
    mov r1, 2
#call("A")
    sto r2, r1
    sto r1, r4
    inc r1
    sto r1, r6
    mov r1, 2
#call("A")
    inc r2
    sto r2, r1
    dec r2
    sto r1, r5
    inc r1
    sto r1, r6
    mov r1, r2
    vxcall sto *changed, 1
    jmp %not_bi
@notS
    rcl r2, r1
    cge r2, 2
    cjz %not_bi
    mov r3, r1
    rcl r1, r3
#call("eval")
    sto r3, r1
    inc r3
    rcl r1, r3
#call("eval")
    sto r3, r1
    dec r3
    mov r1, r3
@not_bi
    pop r3
    pop r2
    ret
