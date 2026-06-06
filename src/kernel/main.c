#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limine.h>
#include <include.h>
#include <pic.h>

// Set the base revision to 6 (the latest)

__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(6);

// Frambuffer request

__attribute__((used, section(".limine_requests")))
volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
    .revision = 0
};

__attribute__((used, section(".limine_requests")))
volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST_ID,
    .revision = 0
};

// The start and end markers for the Limine requests.

__attribute__((used, section(".limine_requests_start")))
static volatile uint64_t limine_requests_start_marker[] = LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile uint64_t limine_requests_end_marker[] = LIMINE_REQUESTS_END_MARKER;

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// Kernel Entry ///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
void kmain(void) {
    __asm__("cli");
    // Ensure the bootloader actually understands our base revision (Support it)
    if (LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision) == false) {
        hcf();
    }
    framebuffer_init() ;

    cls(0x0A0F1F);

    for (int i = 0; i < 4; i++)
        draw_horizental_line(20 + i, 0x00BFFF);

    for (int i = 0; i < 4; i++)
        draw_horizental_line(framebuffer->height - 20 - i, 0x00BFFF);

    barqos_boot_splash();
    print_step("[ OK ] Framebuffer Initialized", 0x00FF00, 1);
    gdt_init();
    print_step("[ OK ] GDT Loaded", 0x00FF00, 1);
    idt_init();
    print_step("[ OK ] IDT Loaded", 0x00FF00, 1);
    disable_apic();
    IRQ_Intialize_PIC(); 
    print_step("[ OK ] PIC Remapped", 0x00FF00, 1);
    APIC_Intialize();
    print_step("[ OK ] Kernel Ready", 0x00FF00, 1);

    current_y += 20;

    print_step("Starting BarqOS...", 0xFFFF00, 1);

    current_y += 160; 
    

    //__asm__ volatile ("sti");

    cursor_x = 1;
    cursor_y = 1;
    // We're done, just hang...
    while(1) {
        __asm__ volatile("hlt");
    }
}

//////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
// Halt and catch fire function.
void hcf(void) {
    for (;;) {
        asm ("hlt");
    }
}

void disable_apic() {
    __asm__ volatile (
        "mov $0x1B, %%rcx;"
        "rdmsr;"
        "and $~0x800, %%rax;"
        "wrmsr;"
        :
        :
        : "rax", "rcx", "rdx"
    ); 
}