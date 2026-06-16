#include "isr.h"
#include "idt.h"
#include "port.h"
#include <screen.h>

extern void c_divide_by_zero_handler();
extern void c_keyboard_handler();
extern void IRQs_Handler(Registers *regs);

void BarqOS_ISR_InitializeGates() {
  for (int i = 0; i < 256; i++) {
    SET_IDT_ENTRY(i, (void *)isr_stub_table[i], 0x08, 0x8E);
  }
}

void isr_handler(Registers *regs) {
  // CPU Exceptions (0x00 - 0x1F)
  if (regs->int_no < 0x20) {
    hal_cls(0x4A0000);
    current_y = 267;
    hal_print_centered(" !!! BARQ OS - KERNEL PANIC !!! ", 0xFFFFFF, 1);
    hal_print_centered("SYSTEM HALTED DUE TO UNHANDLED EXCEPTION:", 0xFFC107,
                       1);
    hal_print_centered("Vector -> ", 0xFFC107, 1);
    char buf[16];
    uint_to_string(regs->int_no, buf);
    hal_print(buf, 0xFFFFFF, 1);
    hal_print_centered("The processor was forced to halt.", 0xDDDDDD, 1);
    while (1)
      ;
  }

  // Hardware Interrupts (0x20+)
  if (regs->int_no >= 0x20 && regs->int_no < 0x30) {
    IRQs_Handler(regs);
  }
}