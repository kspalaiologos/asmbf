
; A hexadecimal counter.

$(
    function pass_around(reg)
        _("inc " .. reg .. "\n")
        _("ceq " .. reg .. ", " .. to_string(string.byte(9) + 1) .. "\n")
        _("cmov " .. reg .. ", .A\n")
    end
)

mov r4, .0
mov r3, r4

@ol
    out r3
    @il
        out r4
        $(pass_around("r4"))
        cne r4, .F
        cjnz %il
    $(pass_around("r3"))
    cne r3, .F
    cjnz %ol
