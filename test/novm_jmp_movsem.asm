
mov r4, .0
mov r5, .1

    mov r1, 3
    mov r2, 5
    ^lt r1, r2
    ^jnz r1, %ok
    out r4
@ok
    out r5
    mov r1, 5
    mov r2, 8
    ^gt r1, r2
    ^jz r1, %ok2
    out r4
@ok2
    out r5
    mov r1, 8
    mov r2, 5
    lt r1, r2
    ^jnz r1, %bad
    out r5
    mov r1, 8
    ^gt r1, r2
    ^jz r1, %bad
    out r5
    ^jmp %over
@bad
    out r4
@over
    out r5
