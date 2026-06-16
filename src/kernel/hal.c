#include <hal.h>

void hal_init() {
  __asm__("cli");
  gdt_init();
  idt_init();
  for (int i = 0; i < 4; i++)
    draw_horizental_line(20 + i, 0x00BFFF);

  for (int i = 0; i < 4; i++)
    draw_horizental_line(framebuffer->height - 20 - i, 0x00BFFF);

  barqos_boot_splash();
  disable_apic();
  IRQ_Intialize_PIC();
}

void hal_print(const char *str, uint32_t color, int scale) {
  print(str, color, scale);
}

void hal_print_centered(char *text, uint32_t color, int scale) {
  print_step(text, color, scale);
}

void hal_print_dec(uint64_t num, uint32_t color, int scale) {
  print_dec(num, color, scale);
}

void hal_print_hex(uint64_t num, uint32_t color, int scale) {
  print_hex(num, color, scale);
}

void hal_cls(uint32_t color) { cls(color); }
