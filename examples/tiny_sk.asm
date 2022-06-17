
?AX=r1
?BX=r2
?CX=r3
?DX=r4
?EX=r5
?FX=r6
?GX=f2
?HX=f3

stk 38
org 0

&heap
db 2

&changed

$(
    function def_inline(ins)
        print("@" .. ins .. "_i\n" .. ins .. " GX, HX\nret\n")
    end

    function inline(ins, arg1, arg2)
        print("mov GX, " .. arg1 .. "\nmov HX, " .. arg2 .. "\n")
        call(ins .. "_i")
    end

    function inline_r(ins, arg1, arg2)
        print("mov HX, " .. arg2 .. "\n")
        call(ins .. "_i")
        print("mov " .. arg1 .. ", GX")
    end
)

#call("read")
@loop
#call("eval")
    $(inline_r("rcl", "BX", "*changed"))
    mov GX, *changed
    mov HX, 0
#call("sto_i")
    ^jnz BX, %loop
    ^push 0
@P
    ^push BX
    add AX, 2
    $(inline_r("rcl", "BX", "AX"))
    sub AX, 2
    jz BX, %PS
    dec BX
    jz BX, %PK
    out .(
    ^mov BX, AX
    $(inline_r("rcl", "AX", "BX"))
#call("P")
    inc BX
    $(inline_r("rcl", "AX", "BX"))
#call("P")
    out .)
    pop BX
    ret
@PS
    out .S
    pop BX
    ret
@PK
    out .K
    pop BX
    ret
@A
    mov HX, *heap
#call("rcl_i")
    add GX, 2
    ^mov HX, AX
#call("sto_i")
    inc GX
    ots GX, *heap
    sub GX, 3
    ^mov AX, GX
    ret
@read
    ^push BX
    in BX
    clr AX
    ceq BX, .S
    cjnz %RK
    ceq BX, .K
    cadd AX, 1
    cjnz %RK
    add AX, 2
#call("A")
    mov BX, AX
#call("read")
    $(inline("sto", "BX", "AX"))
#call("read")
    inc BX
    $(inline("sto", "BX", "AX"))
    dec BX
    ^mov AX, BX
    in BX
    pop BX
    ret
@RK
#call("A")
    pop BX
    ret
@eval
    ^push CX
    $(inline_r("rcl", "BX", "AX"))
    jz BX, %skip
    $(inline_r("rcl", "CX", "BX"))
    jz CX, %skip
    add CX, 2
    $(inline_r("rcl", "DX", "CX"))
    cne DX, 1
    cjnz %skip
    inc BX
    $(inline_r("rcl", "CX", "BX"))
    mov AX, CX
    mov GX, *changed
    mov HX, 1
#call("sto_i")
    jmp %not_bi
@skip
    $(inline_r("rcl", "BX", "AX"))
    jz BX, %notS
    $(inline_r("rcl", "CX", "BX"))
    jz CX, %notS
    $(inline_r("rcl", "DX", "CX"))
    jz DX, %notS
    add DX, 2
    $(inline_r("rcl", "BX", "DX"))
    jnz BX, %notS
    inc AX
    $(inline_r("rcl", "FX", "AX"))
    dec AX
    $(inline_r("rcl", "BX", "AX"))
    $(inline_r("rcl", "CX", "BX"))
    inc BX
    inc CX
    $(inline_r("rcl", "EX", "BX"))
    $(inline_r("rcl", "DX", "CX"))
    mov AX, 2
#call("A")
    ^mov BX, AX
    mov AX, 2
#call("A")
    $(inline("sto", "BX", "AX"))
    $(inline("sto", "AX", "DX"))
    inc AX
    $(inline("sto", "AX", "FX"))
    mov AX, 2
#call("A")
    inc BX
    $(inline("sto", "BX", "AX"))
    dec BX
    $(inline("sto", "AX", "EX"))
    inc AX
    $(inline("sto", "AX", "FX"))
    mov AX, BX
    mov GX, *changed
    mov HX, 1
#call("sto_i")
    jmp %not_bi
@notS
    $(inline_r("rcl", "BX", "AX"))
    cge BX, 2
    cjz %not_bi
    ^mov CX, AX
    $(inline_r("rcl", "AX", "CX"))
#call("eval")
    $(inline("sto", "CX", "AX"))
    inc CX
    $(inline_r("rcl", "AX", "CX"))
#call("eval")
    $(inline("sto", "CX", "AX"))
    dec CX
    ^mov AX, CX
@not_bi
    pop CX
    ret

#def_inline("sto")
#def_inline("rcl")