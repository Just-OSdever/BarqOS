#include <include.h>

// Kernel Entry
void kmain(void) {
  if (LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision) == false) {
    hcf();
  }
  framebuffer_init();
  hal_cls(0x0A0F1F);
  hal_init();
  pmm_init();
  // Test alloc
  *test1 = pmm_alloc(16);
  *test2 = pmm_alloc(64);
  *test3 = pmm_alloc(4096);
  *test4 = pmm_alloc(123);
  *test5 = pmm_alloc(8192);
  // Write there, if (#PF) {There is a problem}
  test1 = 1;
  test2 = 2;
  test3 = 3;
  test4 = 4;
  test5 = 5;

  hal_print("\n\nStarting BarqOS... working \n", 0xFFFF00, 1);

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