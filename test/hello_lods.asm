
stk 2
org 0

&string
txt "Hello World!"
db 0

mov r1, *string
#call("puts")

end

@puts
    lods r2, r1
    jz r2, %puts_quit
    out r2
    jmp %puts
@puts_quit
    out 10
    ret
