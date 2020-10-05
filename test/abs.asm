mov r1, $(signed(-5))
abs r1
eq r1, $(signed(5))
add r1, .0
out r1

mov r1, $(signed(5))
abs r1
eq r1, $(signed(5))
add r1, .0
out r1

mov r1, $(signed(0))
abs r1
eq r1, $(signed(0))
add r1, .0
out r1
