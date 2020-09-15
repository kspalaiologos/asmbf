
; Example RedPower kernel.
; Released to the Public domain.

; Kernel base adress, 1M.
$(KERNEL_BASE = 0x100000)

; Brainfuck tape base adress, right at the 16M mark.
$(TAPE_BASE = 0x1000000)

; Video memory base adress.
$(VIDEO_BASE = 0xA0000)

; Loading the BIOS driver.
$(
    Memory = {}

    Memory.PokeW = (function(absolute_adress, value)
        local function emit(code)
            code:gsub(".", function(c)
                print("raw ." .. c)
            end)
        end

        local function navigate(delta)
            if delta % 2 ~= 0 then
                error("Unsupported access granularity.")
            end

            if delta > 0 then
                emit(">" .. (delta / 2))
            elseif delta < 0 then
                emit("<" .. math.abs(delta / 2))
            end
        end

        local function poke_here(value)
            if value ~= 0 then
                emit("[-]+" .. value)
            else
                emit("[-]")
            end
        end

        local function poke_delta(adress, value)
            navigate(adress)
            poke_here(value)
            navigate(-adress)
        end

        if type(value) == 'number' then
            print("nav r1")
            poke_delta(-10 - (TAPE_BASE - absolute_adress), value)
        else
            print("nav r1")
            navigate(-10 - (TAPE_BASE - absolute_adress))

            for k, c in ipairs(value) do
                poke_here(c)
                emit(">")
            end

            navigate(10 + (TAPE_BASE - absolute_adress) - 2 * #value)
        end
    end)

    local driver = require('./drivers/bios_driver')
    local head = table.remove(driver, 1)

    Memory.PokeW(0x0902, driver)
    Memory.PokeW(0x0900, head)
)

; I/O stack helpers
$(
    IOStack = {}
    
    function IOStack.pop(target)
        print("nav " .. target)
        print("raw .?")
    end

    function IOStack.push(from)
        if type(from) == 'string' then
            print("nav " .. from)
            print("raw .!")
        elseif type(from) == 'number' then
            print("mov f3, " .. from)
            print("nav f3")
            print("raw .!")
        else
            error("Unsupported push: " .. from)
        end
    end

    function IOStack.flush()
        print("raw .#")
    end
)

; Set up a 16 word stack.
stk 32
org 0

; Generate a table of interrupt parameters.
$(
    (function()
        local registers = {'eax', 'ebx', 'ecx', 'edx', 'esi', 'edi', 'ebp', 'eflags'}
        
        for k,v in pairs(registers) do
            print("&i386_h" .. v)
            print("db 0")
            print("&i386_l" .. v)
            print("db 0")
        end
    end)()
)

; Wait until the driver installs itself.
@drv_busy
    mov r1, 1337
    raw .!
    clr r1
    raw .?
    ceq r1, 1337
    cjnz %drv_busy

&message
txt "'The cat is purring, it scratches my skin.\n\r So what is wrong with another sin?'"
db 0

mov r1, *message
mov r2, $(0xC)

#call("puts")

end

; -----------------------------------------------------------------------
; Display a string.
; r1 => string pointer, terminated with a nul.
; r2 => the color.
; Clobbers r1 and r3.
@puts
    rcl r3, r1
    jz r3, %puts_quit
    xchg r1, r3
#call("putchar")
    xchg r1, r3
    inc r1
    jmp %puts
@puts_quit
    out 10
    ret

; -----------------------------------------------------------------------
; Display a character.
; Obviously all the clutter above can be simplified, but we want to stay
; descriptive for now.

; r1 => character
; r2 => color
@putchar
    ; eax = 00000E30h
    vxcall sto *i386_heax, $(0x0000)

    ; Note: you HAVE to use vxcall here, either that or ots.
    ;       The ots approach may be more performant.
    add r1, $(0x0E00)
    vxcall sto *i386_leax, r1
    sub r1, $(0x0E00)

    ; ebx = 0000000Ch
    vxcall sto *i386_hebx, $(0x0000)
    vxcall sto *i386_lebx, r2

    ; ecx = 0000000Ch
    vxcall sto *i386_hecx, $(0x0000)
    vxcall sto *i386_lecx, $(0x0000)

    ; edx = 0000000Ch
    vxcall sto *i386_hedx, $(0x0000)
    vxcall sto *i386_ledx, $(0x0000)

    ; esi = 0000000Ch
    vxcall sto *i386_hesi, $(0x0000)
    vxcall sto *i386_lesi, $(0x0000)

    ; edi = 0000000Ch
    vxcall sto *i386_hedi, $(0x0000)
    vxcall sto *i386_ledi, $(0x0000)

    ; ebp = 0000000Ch
    vxcall sto *i386_hebp, $(0x0000)
    vxcall sto *i386_lebp, $(0x0000)

    ; eflags = 0000000Ch
    vxcall sto *i386_heflags, $(0x0000)
    vxcall sto *i386_leflags, $(0x0000)

    ; BIOS function 0x10
    mov r1, $(0x10)

    $(call("int386"))
    ret

; -----------------------------------------------------------------------
; Fire an interrupt.
; Takes input via global variables, stores the state in a separate
; structure. The interrupt vector is accepted in `r1'.

; Possible bugs: Doesn't save/restore high eax bits or low eflags bits.

; I/O stack state _before_ the interrupt:

;  top                                                            bottom
;  +-----+--------+-----+-----+-----+-----+-----+-----+-----+----------+
;  | vec | eflags | ebp | edi | esi | edx | ecx | ebx | eax | function | 
;  +-----+--------+-----+-----+-----+-----+-----+-----+-----+----------+
; 36    34       30    26    22    18    14    10     6     2          0
;         \-------------------------------------------------/
;          encoded as two stack elements, where each slot is
;               represented with it's highest bits low.

; I/O stack state _after_ the interrupt:

;  top                                           bottom
;  +--------+-----+-----+-----+-----+-----+-----+-----+
;  | eflags | ebp | edi | esi | edx | ecx | ebx | eax |
;  +--------+-----+-----+-----+-----+-----+-----+-----+
; 32       28    24    20    16    12     8     4     0

@int386
    xchg r1, r3
    push r1
    push r2
    $(IOStack.push(0x1))
    mov r2, *i386_heax
@pushloop
    rcl r1, r2
    $(IOStack.push("r1"))
    cge r2, *i386_leflags
    inc r2
    cjz %pushloop
    $(IOStack.push("r3"))
    $(IOStack.flush())
    $(IOStack.pop("r1"))
    $(IOStack.pop("r1"))
    mov r2, *i386_leflags
@poploop
    $(IOStack.pop("r1"))
    sto r2, r1
    ceq r2, *i386_heax
    dec r2
    cjz %poploop
    pop r2
    pop r1
    xchg r3, r1
    ret
