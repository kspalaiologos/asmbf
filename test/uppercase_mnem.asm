
STK 2
ORG 0

TXT "Hello World!"
DB_ 0

@LOOP
    RCL R2, R1
    JZ R2, 0
    OUT R2
    INC R1
    JMP %LOOP
