#!/bin/asmbf/bfmake
mov r1, 2
clt r1, 3
cmu r1, 32
cmd r1, 10
add r1, .0
out r1
ceq r1, 0
raw .0
cjz 0
out .!