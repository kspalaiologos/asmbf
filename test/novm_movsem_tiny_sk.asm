
stk 38
org 0

&heap
db 2

&changed

$(
    function def_inline(ins)
        print("@" .. ins .. "_i\n" .. ins .. " f2, f3\nret\n")
    end

    function inline(ins, arg1, arg2)
        print("mov f2, " .. arg1 .. "\nmov f3, " .. arg2 .. "\n")
        call(ins .. "_i")
    end

    function inline_r(ins, arg1, arg2)
        print("mov f3, " .. arg2 .. "\n")
        call(ins .. "_i")
        print("mov " .. arg1 .. ", f2")
    end
)

#call("read")
@loop
#call("eval")
    $(inline_r("rcl", "r2", "*changed"))
    mov f2, *changed
    mov f3, 0
#call("sto_i")
    ^jnz r2, %loop
    ^push 0
@P
    ^push r2
    add r1, 2
    $(inline_r("rcl", "r2", "r1"))
    sub r1, 2
    jz r2, %PS
    dec r2
    jz r2, %PK
    out .(
    ^mov r2, r1
    $(inline_r("rcl", "r1", "r2"))
#call("P")
    inc r2
    $(inline_r("rcl", "r1", "r2"))
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
    mov f3, *heap
#call("rcl_i")
    add f2, 2
    ^mov f3, r1
#call("sto_i")
    inc f2
    ots f2, *heap
    sub f2, 3
    ^mov r1, f2
    ret
@read
    ^push r2
    in r2
    ceq r2, .S
    cjnz %RS
    ceq r2, .K
    cjnz %RK
    mov r1, 2
#call("A")
    mov r2, r1
#call("read")
    $(inline("sto", "r2", "r1"))
#call("read")
    inc r2
    $(inline("sto", "r2", "r1"))
    dec r2
    ^mov r1, r2
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
    ^push r2
    ^push r3
    $(inline_r("rcl", "r2", "r1"))
    jz r2, %skip
    $(inline_r("rcl", "r3", "r2"))
    jz r3, %skip
    add r3, 2
    $(inline_r("rcl", "r4", "r3"))
    cne r4, 1
    cjnz %skip
    inc r2
    $(inline_r("rcl", "r3", "r2"))
    mov r1, r3
    mov f2, *changed
    mov f3, 1
#call("sto_i")
    jmp %not_bi
@skip
    $(inline_r("rcl", "r2", "r1"))
    jz r2, %notS
    $(inline_r("rcl", "r3", "r2"))
    jz r3, %notS
    $(inline_r("rcl", "r4", "r3"))
    jz r4, %notS
    add r4, 2
    $(inline_r("rcl", "r2", "r4"))
    jnz r2, %notS
    inc r1
    $(inline_r("rcl", "r6", "r1"))
    dec r1
    $(inline_r("rcl", "r2", "r1"))
    $(inline_r("rcl", "r3", "r2"))
    inc r2
    inc r3
    $(inline_r("rcl", "r5", "r2"))
    $(inline_r("rcl", "r4", "r3"))
    mov r1, 2
#call("A")
    ^mov r2, r1
    mov r1, 2
#call("A")
    $(inline("sto", "r2", "r1"))
    $(inline("sto", "r1", "r4"))
    inc r1
    $(inline("sto", "r1", "r6"))
    mov r1, 2
#call("A")
    inc r2
    $(inline("sto", "r2", "r1"))
    dec r2
    $(inline("sto", "r1", "r5"))
    inc r1
    $(inline("sto", "r1", "r6"))
    mov r1, r2
    mov f2, *changed
    mov f3, 1
#call("sto_i")
    jmp %not_bi
@notS
    $(inline_r("rcl", "r2", "r1"))
    cge r2, 2
    cjz %not_bi
    ^mov r3, r1
    $(inline_r("rcl", "r1", "r3"))
#call("eval")
    $(inline("sto", "r3", "r1"))
    inc r3
    $(inline_r("rcl", "r1", "r3"))
#call("eval")
    $(inline("sto", "r3", "r1"))
    dec r3
    ^mov r1, r3
@not_bi
    pop r3
    pop r2
    ret

#def_inline("sto")
#def_inline("rcl")
