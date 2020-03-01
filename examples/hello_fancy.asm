
; A fancy hello world program utilizing the CALLIB, and the newest label features
; including data labels and other goodies we've got!

#define _CALLIB_CAT(a, ...) _CALLIB_PRIMITIVE(a, __VA_ARGS__)
#define _CALLIB_PRIMITIVE(a, ...) a ## __VA_ARGS__

#define call(fn) \
    psh _CALLIB_CAT(%call_return_, __ICOUNTER__) __NEWLINE__ \
    jmp %fn __NEWLINE__ \
    _CALLIB_CAT(@call_return_, __CCOUNTER__)

stk 2
org 2

&string
txt "Hello World!"
db_ 0

mov r1, *string
call(puts)

end

@puts
    rcl r2, r1
    jz_ r2, %puts_quit
    out r2
    inc r1
    jmp %puts
@puts_quit
    out 10

