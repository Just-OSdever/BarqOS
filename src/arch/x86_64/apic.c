#include "apic.h"
#define IA32_APIC_BASE_MSR 0x1B

void APIC_Intialize() {
    uint32_t status = detect_apic();
    if (status == 0) {
        // no support, complete by PIC!
        print_step("no support" , 0xffffff , 1);
        IRQ_Intialize_PIC();
    }
    else if (status == 1) {
        // disabled
        print_step("disabled" , 0xffffff , 1);
        outb(0x20 , 0xFF); //check that PIC is not working
        outb(0x21 , 0xFF);
        enable_apic();
    }
    else if (status == 2) {
        // enabled! , but check it again
        print_step("enabled" , 0xffffff , 1);
        outb(0x20 , 0xFF); //check that PIC is not working
        outb(0x21 , 0xFF);
        enable_apic();
    }
    else {
        print_step("wtf is that, there is a problem in apic, it isn't disabled,enabled or supported" , 0xffffff , 1);
    }
}


void GetMSR(uint32_t msr, uint32_t *lo, uint32_t *hi)
{
   asm volatile("rdmsr" : "=a"(*lo), "=d"(*hi) : "c"(msr));
}

void SetMSR(uint32_t msr, uint32_t *lo, uint32_t *hi)
{
   asm volatile("wrmsr" : : "a"(*lo), "d"(*hi), "c"(msr));
}

// Commented for understanding next code session

/*
uintptr_t cpu_get_apic_base() {
   uint32_t eax, edx;
   cpuGetMSR(IA32_APIC_BASE_MSR, &eax, &edx);

#ifdef __PHYSICAL_MEMORY_EXTENSION__
   return (eax & 0xfffff000) | ((edx & 0x0f) << 32);
#else
   return (eax & 0xfffff000);
#endif
}
*/

void enable_apic() {
    // Let's enable apic
    print_step("apic will be enabled now!" , 0xffffff , 1);
}
