[bits 64]

global gdt_flush

section .data
align 16

gdt64:
    ; 1. Null Descriptor (8 bytes)
    dq 0
    ; 2. Kernel Code Segment
    dw 0xFFFF       ; Limit Low (16 bits)
    dw 0x0000       ; Base Low (16 bits)
    db 0x00         ; Base Middle (8 bits)
    db 0x9A         ; Access Byte (8 bits)
    db 0xAF         ; Granularity (4 bits) + Limit High (4 bits)
    db 0x00         ; Base High (8 bits)
    ; 3. Kernel Data Segment
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x92
    db 0xCF
    db 0x00
    ; 4. User Code Segment
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0xFA
    db 0xAF
    db 0x00
    ; 5. User Data Segment
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0xF2
    db 0xAF
    db 0x00

gdt_end:

gdtr:
    dw gdt_end - gdt64 - 1
    dq gdt64

section .text

gdt_flush:
    lgdt [rel gdtr]
    mov ax , 0x10 ;Kernel Mode Data Segment
    mov ds , ax ;Data segment = 0x10 
    ;load other data segments
    mov es, ax
    mov fs, ax
    mov gs, ax
    ;mov ss, ax not in Long Mode
    ;no jmp 0x08:flush2 in 64-bits ^ Long Mode ^
    push 0x08
    lea rax , [rel flush2]
    push rax
    retfq
flush2:
    ret