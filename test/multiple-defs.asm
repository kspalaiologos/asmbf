&a
db 1
&b
db 2
&c
db 3
rcl r1, *a
rcl r2, *b
rcl r3, *c
add r1, .0
add r2, .0
add r3, .0
out r1
out r2
out r3
