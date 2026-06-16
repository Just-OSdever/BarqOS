#include <include.h>

// Kernel Entry
void kmain(void) {
  if (LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision) == false) {
    hcf();
  }
  framebuffer_init();
  hal_cls(0x0A0F1F);
  lzml_init();
  hal_cls(0x0A0F1F);
  hal_init();
  current_y += 20;

  hal_print_centered("Starting BarqOS... working", 0xFFFF00, 1);
  hal_print("\n", 0xFFFFFF, 1);
  // We're done, just hang...
  hcf();
}

// Halt and catch fire function.
void hcf(void) {
  for (;;) {
    asm("hlt");
  }
}