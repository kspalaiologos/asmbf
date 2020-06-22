
txt "abcdefghijklmnop"

mov r5, 2
mov r6, 3
rcl r1, 1(r6,r5,4)
out r1

; mov r1, 2(r3, r4, 4) => mov r1, [r4 * 4 + r3 + 2]
