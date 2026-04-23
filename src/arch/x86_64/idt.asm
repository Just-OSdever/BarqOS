[bits 64]
default rel

global idt_init

section .bss
align 16
idt: ;IDT table
    resb 256 * 16    ; 256 entries, each 16 bytes (Interrupt Gate)

section .data
idtr:
    dw (256 * 16) - 1    ;limit (first 2 bytes) = (size - 1)
    dq idt              ;base addr of IDT table

section .text
%macro set_gate 3

    mov rax, %2
    lea rbx, [rel idt] ;get the first gate addr in ^ Long Mode ^
    add rbx, (%1 * 16) ;move to the gate giveen in %1

    ; Offset Low
    mov [rbx], ax
    ; Selector
    mov word [rbx + 2], %3
    ; Flags (0x8E = Interrupt Gate)
    mov byte [rbx + 5], 0x8E
    ; Offset Mid
    shr rax, 16
    mov [rbx + 6], ax
    ; Offset High
    shr rax, 16
    mov [rbx + 8], eax
%endmacro

; simple IDT_INIT
extern soft_interrupt_handler_c

idt_init:
    ; our handlers
    set_gate 3, software_handler_wrapper, 0x08 ;the OSdeving interruppt
    ;set_gate 1, keyboard_handler, 0x08
    
    lidt [rel idtr]
    ret

software_handler_wrapper:
    ; Save registers
    push rax
    push rcx
    push rdx
    push rsi
    push rdi

    mov rdi, rsp
    call soft_interrupt_handler_c

    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rax

    ; الحل هنا: لازم نضيف SS و RSP يدوياً عشان الـ iretq يلاقي الـ 5 قيم
    mov rax, ss
    push rax            ; ن push الـ SS الأصلي
    push rsp            ; ن push الـ RSP الأصلي (قبل ما نعمل push للـ SS)
    
    ; لازم نعدل الـ RSP عشان الـ iretq ي pop القيم اللي ضفناها
    ; لاحظ: بعد الـ push للـ SS والـ RSP، الـ RSP اتغير، لازم نرجع نظبطه
    iretq