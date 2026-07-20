#include <hal.h>

void hal_init() {
  __asm__("cli");
  gdt_init();
  idt_init();
  barqos_boot_splash();
  disable_apic();
  IRQ_Intialize_PIC();
}

void hal_print(const char *str, uint32_t color, int scale) {
  print(str, color, scale);
}

void hal_print_dec(uint64_t num, uint32_t color, int scale) {
  print_dec(num, color, scale);
}

void hal_print_hex(uint64_t num, uint32_t color, int scale) {
  print_hex(num, color, scale);
}

void hal_cls(uint32_t color) { cls(color); }

void hal_clear_line(uint32_t y, uint32_t height) {
  clear_line( y, height );
}