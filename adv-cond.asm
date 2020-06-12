
; r1 = r2 = r3
; r3 >= r2
; r3 <= r4

in r1
in r2
in r3
in r4

ceq r1, r2
cout .1

candeq r2, r3
cout .1

candlt r2, r3
cout .0

coreq r1, r2
cout .1

candgt r3, r4
cout .0

; r1 < r2
; r2 >= r3
; r3 < r4

in r1
in r2
in r3
in r4

corne r1, r2
cout .1

candge r2, r3
cout .1

corlt r3, r4
cout .1

cflip

corgt r1, r2
cout .0

corle r3, r4
cout .1

; r1 < r2
; r2 < r3
; r3 = r4

in r1
in r2
in r3
in r4

cxorge r1, r2
cout .1

cxorle r2, r3
cout .0

cxoreq r3, r4
cout .1
