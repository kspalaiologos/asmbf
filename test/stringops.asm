
; put 5 and 6 consecutively in memory
stos r1, 2
stos r1, 3
; load both of these again
lods r1, r2
lods r3, r2
; print 5
add r1, r3
add r1, .0
out r1
; check if scas works
clr r1
scas r1, 2
cout .1
scas r1, 3
cout .1
