
; Sierpinski triangle fractal viewer


@loop1
    mov r2, 0
    
    @loop2
        mov r3, r1
        band r3, r2
        cge r3, 1
        mov r3, .*
        cmov r3, 32
        out r3
        
        clt r2, 63
        cadd r2, 1
        cjn %loop2
    
    out 10
    
    clt r1, 63
    cadd r1, 1
    cjn %loop1
