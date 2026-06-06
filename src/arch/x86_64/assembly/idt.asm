[bits 64]
[default rel]

; C :
; void IDT_load(IDTDescriptor* idtDescriptor);


global IDT_load
IDT_load:
    ; load idt
    lidt [rdi]              ; loading first argument
    o64 ret

extern c_divide_by_zero_handler 

global divide_by_zero_handler
divide_by_zero_handler:
    push rbp
    mov rbp, rsp

    push rdi
    push rsi
    push rdx
    push rcx
    push r8
    push r9
    push rax
    
    call c_divide_by_zero_handler
    
    pop rax
    pop r9
    pop r8
    pop rcx
    pop rdx
    pop rsi
    pop rdi

    pop rbp
    iretq