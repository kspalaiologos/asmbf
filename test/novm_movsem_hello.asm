
&text
txt "Hello, world!\n"

@print_loop
    rcl r2, r1
    inc r1
    out r2
    ^eq r2, 10
    ^jz r2, %print_loop
