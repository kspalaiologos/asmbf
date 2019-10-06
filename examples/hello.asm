
stk     2               ; set up the stack with 2 elements max
org     0
txt     "Hello World!"
db_     0
        psh     1       ; return address
        psh     0       ; address of text string
        jmp     100     ; call puts
lbl 1
        end             ; end program

; Damn simple PUTS implementation

lbl 100
        pop     r1
lbl 102
        rcl     r2,r1
        jz_     r2,101
        out     r2
        inc     r1
        jmp     102
lbl 101
        out     10
        ret
