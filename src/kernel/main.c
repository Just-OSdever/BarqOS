#include <include.h>

// Kernel Entry
void kmain(void) {
  if (LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision) == false) {
    hcf();
  }
  framebuffer_init();
  hal_cls(0x000000);
  hal_init();
  pmm_init();
  // Test alloc
  allocator_test();

  hal_print("Yeaaaaaaaaa, good." , 0xff00ff , 1);
  // hal_cls(0x000000);

  // hal_print("BarqOS->/No FileSystem yet : ", 0xFFFF00, 2);

  //__asm__ volatile ("sti");

  // We're done, just hang...
  hcf();
}

// Halt and catch fire function.
void hcf(void) {
  for (;;) {
    asm("hlt");
  }
}