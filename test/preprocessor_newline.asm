#!/bin/bfmake
#define call(L, R) psh %L##R __NEWLINE__ jmp %L __NEWLINE__ @L##R
call(label, 1)
end
@label
out .1
ret