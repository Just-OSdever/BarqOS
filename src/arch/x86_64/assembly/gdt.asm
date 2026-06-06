[bits 64]
[default rel]

; C :
; gdt_load( &gdt_descriptor , 0x08 , 0x10 );


global gdt_load
gdt_load:
    ; load GDT
    lgdt [rdi]              ; loading it, first argument (&gdt_descriptor)

    ;far jump
    push rsi                ; pushing the kernel code segment
    lea rax, reload_cs      ; the label
    push rax                ; The RIP
    o64 retf                ; The far jump will take : CS then RIP (o64 to force it to use 64-bit)

reload_cs:
    mov ax , dx
    mov ds , ax
    mov ss , ax
    mov es , ax
    mov fs , ax
    mov gs , ax

    mov ax , 0x28
    ltr ax

    ret

