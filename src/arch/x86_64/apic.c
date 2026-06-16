#include "apic.h"

#define IA32_APIC_BASE_MSR 0x1B
#define IA32_APIC_BASE_MSR_ENABLE 0x800
#define APIC_BASE_PHYSICAL 0xFEE00000

uint64_t hhdm_offset = 0;

void APIC_Intialize() {
  if (hhdm_request.response != NULL) {
    hhdm_offset = hhdm_request.response->offset;
  } else {
    hal_print_centered("Limine HHDM response is NULL! Configuration error.",
                       0xff0000, 1);
    return;
  }
  uint32_t status = detect_apic();
  if (status == 0) {
    // no support, complete by PIC!
    hal_print_centered("no support", 0xffffff, 1);
    IRQ_Intialize_PIC();
  } else if (status == 1) {
    // disabled
    hal_print_centered("disabled", 0xffffff, 1);
    outb(0x20, 0xFF); // check that PIC is not working
    outb(0x21, 0xFF);
    enable_apic();
  } else if (status == 2) {
    // enabled! , but check it again
    hal_print_centered("enabled", 0xffffff, 1);
    outb(0x20, 0xFF); // check that PIC is not working
    outb(0x21, 0xFF);
    enable_apic();
  } else {
    hal_print_centered("wtf is that, there is a problem in apic, it isn't "
                       "disabled,enabled or supported",
                       0xffffff, 1);
  }
}

__attribute__((noinline)) uint64_t get_apic_base_virtual() {
  uint64_t apic_base_virtual =
      ((uint64_t)hhdm_request.response->offset) + APIC_BASE_PHYSICAL;
  return apic_base_virtual;
}

uint32_t ReadRegister(uint32_t reg) {
  volatile uint32_t *address =
      (volatile uint32_t *)(get_apic_base_virtual() + reg);
  return *address;
}

void WriteRegister(uint32_t reg, uint32_t value) {
  volatile uint32_t *address =
      (volatile uint32_t *)(get_apic_base_virtual() + reg);
  *address = value;
}

void enable_apic() {
  // Let's enable apic
  hal_print_centered("apic will be enabled now!", 0xffffff, 1);
  // enabling bit 11 (global apic enable)
  uint64_t apic_base_msr = GetMSR(IA32_APIC_BASE_MSR);
  SetMSR(IA32_APIC_BASE_MSR, apic_base_msr | IA32_APIC_BASE_MSR_ENABLE);
  // enable bit 8 in Spurious Interrupt Vector Register to enable interrupts
  // and delete the first 7 bits (the vector) then replace them by 255
  WriteRegister(0xf0, (ReadRegister(0xf0)) | 0x100);
}

void disable_apic() {
  __asm__ volatile("mov $0x1B, %%rcx;"
                   "rdmsr;"
                   "and $~0x800, %%rax;"
                   "wrmsr;"
                   :
                   :
                   : "rax", "rcx", "rdx");
}