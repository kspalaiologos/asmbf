[bits 32]
stk 6
org 0
db 0
&name_var
db 4294967295

mov r1, *name_var  
dec r1
