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
  void *test1 = pmm_alloc(16);
  void *test2 = pmm_alloc(64);
  void *test3 = pmm_alloc(4096);
  void *test4 = pmm_alloc(123);
  void *test5 = pmm_alloc(8192);
  // Write there, if (#PF) {There is a problem}
  test1 = (void *)1;
  test2 = (void *)2;
  test3 = (void *)3;
  test4 = (void *)4;
  test5 = (void *)5;

  hal_cls(0x000000);

  hal_print("BarqOS->/Home : ", 0xFFFF00, 2);

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