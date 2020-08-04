#!/sublime/asmbf
txt "abcdefghijklmnop"

mov r5, 2
mov r6, 3
inc 1(r6,r5,4)''
mov r1, 1(r6,r5,4)'
out r1