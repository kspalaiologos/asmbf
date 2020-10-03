
; A fancy hello world program utilizing the CALLIB, and the newest label features
; including data labels and other goodies we've got!

stk 2
org 0

&string
txt "Hello World!"
db 0

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
