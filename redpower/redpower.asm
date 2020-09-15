
; RedPower bootloader for asm2bf.
; (C) 2020 by Kamila Szewczyk, licensed under the terms of MIT license.

[BITS 16]
org 7C00h

; Terrible bit of code to measure the length in sectors
; and round up when needed. Normally I'd use `> 0', but nasm doesn't
; seem to support it, so we're left with `!!'

%define to_sectors(x) x / 512 + !!(x % 512)

start:
    ; Clear segment registers
    ; Set up a small stack.
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x9c00

    xor ah, ah
    mov al, 10h
    int 10h

    cli
    mov ax, 0
    out 0x40, al
    out 0x40, al

    ; Save the drive number for later use.
    mov [drive], dl

    mov si, msgredpower
    call puts
    mov si, msgturntheworld
    call puts

    ; Check if int13h ah=42h is supported for this drive.
    ; TODO: Fall back to legacy reading method if not supported.
    mov ah, 0x41
    mov bx, 0x55aa
    int 0x13
    jc err

    ; A20 code.
    mov ax, 2401h
    int 15h

    mov si, gota20
    call puts

    ; Enter voodoo mode.
    cli
    push ds
    lgdt [gdtinfo]
    mov eax, cr0
    or al, 1
    mov cr0, eax
    jmp $+2
    mov bx, 0x08
    mov ds, bx
    and al, 0xFE
    mov cr0, eax
    pop ds
    sti

    mov si, gotvoodoo
    call puts

    ; Read the 2nd stage bootloader to 0x0000:0x0500.
    mov bx, 0x500

    ; Start from the 2nd sector.
    mov eax, 1
    mov cx, secondstage_size / 512

.loop:
    mov dl, [drive]
    call read_sector
    add bx, 512
    inc eax
    dec cx
    jnz .loop

    mov si, got2ndstage
    call puts

    xor ecx, ecx
    mov cx, to_sectors(kernel_size)
    mov dl, [drive]
    jmp 0x500

err:
    mov si, nofeature
    call puts
    cli
    hlt

puts:
    pusha
    mov ah, 0Eh
    mov bl, byte [si]
    inc si
.loop:
    mov al, byte [si]
    inc si
    cmp al, 0
    je .done
    int 10h
    jmp .loop
.done:
    popa
    ret

gotvoodoo:       db 0xA, "[+] Data descriptors", 10, 13, 0
gota20:          db 0xA, "[+] A20", 10, 13, 0
got2ndstage:     db 0xA, "[+] 2nd stage", 10, 13, 0
nofeature:       db 0xC, "Error: The I/O device doesn't support extended read.", 10, 13, 0
msgredpower:     db 0xC, 10, 13, 10, 13, "RedPower", 0
msgturntheworld: db 0xF, " - turn the world red!", 10, 13, "(C) Kamila Szewczyk, 2020", 10, 10, 13, 0

; Read a sector from LBA in eax, store the data in es:bx.
read_sector:
    pusha

    ; We construct a disk i/o packet on the stack to shave off a few bytes.
    push byte 0
    push byte 0
    push eax
    push es
    push bx
    push byte 1
    push byte 16

    ; Read sectors...
    mov ah, 42h
    mov si, sp
    int 13h

    ; Check if succeeded.
    ; TODO: Retry?
    jc short .read_error

    ; Fix up the stack.
    add sp, 16

    popa
    ret

; Generic error message display.
; TODO: descriptive error message, provided it fits?
.read_error:
    mov ax, 0E00h+'E'
    mov bx, 7
    int 10h
    cli
    hlt

drive db 0

gdtinfo:
   dw gdt_end - gdt - 1
   dd gdt

gdt         dd 0,0
flatdesc    db 0xff, 0xff, 0, 0, 0, 10010010b, 11001111b, 0
gdt_end:
 
   times 510-($-$$) db 0
   dw 0xAA55

; ************************************************************************
; ~~ 2nd stage bootloader code.
; A (hopefully) performant brainfuck interpreter.

secondstage:
    incbin 'boot/interpreter.hex'

secondstage_size equ ($-secondstage)

; ************************************************************************
; ~~ The kernel.

kernel:
    incbin 'kernel/kernel.b'
    kernel_size equ ($-kernel)

%if kernel_size > 0x1000000
    %error 'Kernel size exceeds 16M.'
%endif
