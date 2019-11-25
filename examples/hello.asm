
stk 2
org 0

txt "Hello World!"
db_ 0

@loop
        rcl r2, r1
        jz_ r2, %quit
        out r2
        inc r1
        jmp %loop
@quit
        out 10
