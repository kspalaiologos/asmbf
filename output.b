
; Don't ask questions, this is beyond explaining








; Some common defines






; gcd, cgc, cre



; floating point







; Conditional instructions













































; Allocate a page and pass the pointer in target_register.
; This function has a side effect on register passed in clear.
; It's getting, simply, cleared.

; Free the page in target_register.
	
txt "abcdefghijklmnop"

mov r5, 2
mov r6, 3
mov f2, r5
mul f2, 4
add f2, r6
add f2, 1
rcl f2, f2
out f2
inc f2
out f2

mov f3, r5
mul f3, 4
add f3, r6
add f3, 1
sto f3, f2

mov f2, r5
mul f2, 4
add f2, r6
add f2, 1
rcl f2, f2
mov r1, f2
out r1
