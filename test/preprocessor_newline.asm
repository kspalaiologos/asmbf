#!/bin/bfmake
#define call(L, R) psh %L##R NEWLINE jmp %L NEWLINE @L##R
call(label, 1)
end
@label
out .1
ret