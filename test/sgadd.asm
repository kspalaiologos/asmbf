
mov r1, $(signed(-3))
mov r2, $(signed(-2))
s07 r1, r2
eq r1, $(0xB)
add r1, .0
out r1

mov r1, $(signed(-3))
mov r2, $(signed(2))
s07 r1, r2
eq r1, $(0x3)
add r1, .0
out r1

mov r1, $(signed(3))
mov r2, $(signed(-2))
s07 r1, r2
eq r1, $(0x2)
add r1, .0
out r1

mov r1, $(signed(3))
mov r2, $(signed(2))
s07 r1, r2
eq r1, $(0xA)
add r1, .0
out r1

mov r1, $(signed(-2))
mov r2, $(signed(-3))
s07 r1, r2
eq r1, $(0xB)
add r1, .0
out r1

mov r1, $(signed(2))
mov r2, $(signed(3))
s07 r1, r2
eq r1, $(0xA)
add r1, .0
out r1
