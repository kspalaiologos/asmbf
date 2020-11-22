
#function bf_while(reg) print("nav " .. reg); print("raw .["); end
#function bf_wend(reg) print("nav " .. reg); print("raw .]"); end

    clr r1
    mov r2, 1
$(bf_while("r2"))
    inc r1
    asl r2
$(bf_wend("r2"))

    add r1, .0
    out r1
