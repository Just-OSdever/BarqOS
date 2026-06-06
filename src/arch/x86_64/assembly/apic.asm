; Defines
APIC_NOT_SUPPORTED  equ 0
APIC_DISABLED       equ 1
APIC_ENABLED        equ 2
IA32_APIC_BASE_MSR  equ 0x1B

; Function: detect_apic
; Returns: EAX = APIC status code
global detect_apic

detect_apic:
    push rbx
    push rcx
    push rdx

    ;step 1: Check CPUID for APIC support
    mov eax, 1 ;Features
    CPUID

    test edx , (1 << 11)
    jz .no_support

    ;step 1: Check MSR for APIC enabled
    mov ecx, IA32_APIC_BASE_MSR
    rdmsr

    test eax, APIC_ENABLED
    jz .disabled 

    ;if still here, APIC is enabled
    mov eax, APIC_ENABLED
    jmp .done

extern PICEnabling

.no_support:
    mov eax, APIC_NOT_SUPPORTED
    jmp .done

.disabled:
    mov eax, APIC_DISABLED
    jmp .done

.done:
    pop rdx
    pop rcx
    pop rbx
    ret