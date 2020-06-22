
stk 2
org 0

&string
txt "test\0"

mov r1, *string
#call("puts")

end

@puts
    rcl r2, r1
    jz r2, %puts_quit
    out r2
    inc r1
    jmp %puts
@puts_quit
    out 10
