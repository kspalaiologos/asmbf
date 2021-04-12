
stk 5

; prints 1111321

mbegin
 mov r1, 5
 mov r2, 5
 le r1, r2
 mov r2, 5
 mov r2, 6
 le r1, r2
 and r1, r2
mend

add r1, .0
out r1

mbegin
 mov r1, 4
 mov r2, 2
 le r1, r2
mend

add r1, .0
inc r1
out r1

mov r1, 9
^mod r1, 2
add r1, .0
out r1

^mov r1, 3
^mov r2, 2
^ne r1, r2
^mov r2, 3
^mov r3, 3
^ne r2, r3
log r2
inc r2
^and r1, r2
add r1, .0
out r1

^mov r1, 1
^mov r2, 3

^psh r1
^psh 2
^psh r3

pop r4
add r4, .0
out r4
pop r4
add r4, .0
out r4
pop r4
add r4, .0
out r4

mov r1, 1
mov r2, 1
add r1, r2
add r2, .0
out r2
