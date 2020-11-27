
; build me by executing:
;  > ~/.asmbf/bfmake -t music.asm
;  > ~/.asmbf/bfi music.b > sample_data
; (^C after some time)
;  > cat sample_data | aplay

; You should hear a very basic tune :)

mov r4, 1280
mov r1, 1

raw .[
mov r2, r1
sub r2, r4
bxor r2, r1
shr r2, 10
mov r3, r1
shr r3, 15
mov r5, r3
mod r5, 2
not r5
add r3, r5
add r3, r1
bxor r3, r1
bxor r2, r3
mul r2, r1
out r2
inc r1
raw .]

