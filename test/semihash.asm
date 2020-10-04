; Wrap everything in a loop.
@l
; Read a character from stdin.
    in  r1
; if r1 = ';', set the condition flag.
    ceq r1,.;
; if condition flag is set, add 1 to r2
; r2 is used as an accumulator.
    cad r2,1
; if r1 = '#', set the condition flag.
    ceq r1,.#
; if condition flag is not set, jump to d
    cjz %d
; display acc%127 and clear it.
    mod r2,127
    out r2
    clr r2
@d
; loop if didn't hit an EOF
    jnz r1, %l