
; Absolute:
; [5] = 2
; [6] = 3
; [7] = 4
; Relative to segment 5:
; [0] = 2
; [1] = 3
; [2] = 4

seg 5
db 48
inc r1
sto r1, 49
org 2
db 50
rcl r2, 0
rcl r3, 1
rcl r4, 2
out r2
out r3
out r4

seg 0
rcl r2, 5
rcl r3, 6
rcl r4, 7
out r2
out r3
out r4
