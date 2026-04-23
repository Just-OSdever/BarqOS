#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limine.h>
#include <include.h>


// Set the base revision to 6 (the latest)

__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(6);

// Frambuffer request

__attribute__((used, section(".limine_requests")))
volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
    .revision = 0
};

// The start and end markers for the Limine requests.

__attribute__((used, section(".limine_requests_start")))
static volatile uint64_t limine_requests_start_marker[] = LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile uint64_t limine_requests_end_marker[] = LIMINE_REQUESTS_END_MARKER;


// Copied from OSdev wiki for future understanding & really intalizing them


// GCC and Clang reserve the right to generate calls to the following
// 4 functions even if they are not directly called.
// Implement them as the C specification mandates.
// DO NOT remove or rename these functions, or stuff will eventually break!
// They CAN be moved to a different .c file.

/*void *memcpy(void *restrict dest, const void *restrict src, size_t n) {
    uint8_t *restrict pdest = (uint8_t *restrict)dest;
    const uint8_t *restrict psrc = (const uint8_t *restrict)src;

    for (size_t i = 0; i < n; i++) {
        pdest[i] = psrc[i];
    }

    return dest;
}

void *memset(void *s, int c, size_t n) {
    uint8_t *p = (uint8_t *)s;

    for (size_t i = 0; i < n; i++) {
        p[i] = (uint8_t)c;
    }

    return s;
}

void *memmove(void *dest, const void *src, size_t n) {
    uint8_t *pdest = (uint8_t *)dest;
    const uint8_t *psrc = (const uint8_t *)src;

    if ((uintptr_t)src > (uintptr_t)dest) {
        for (size_t i = 0; i < n; i++) {
            pdest[i] = psrc[i];
        }
    } else if ((uintptr_t)src < (uintptr_t)dest) {
        for (size_t i = n; i > 0; i--) {
            pdest[i-1] = psrc[i-1];
        }
    }

    return dest;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t *p1 = (const uint8_t *)s1;
    const uint8_t *p2 = (const uint8_t *)s2;

    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] < p2[i] ? -1 : 1;
        }
    }

    return 0;
}

*/


////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// Kernel Entry ///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
void kmain(void) {
    // Ensure the bootloader actually understands our base revision (Support it)
    if (LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision) == false) {
        hcf();
    }

    framebuffer_init() ;

    cls(0x121212);

    draw_vertical_line((framebuffer->width ) / 50 ,0xFFC107);       // ْX , color
    draw_vertical_line((49 * framebuffer->width ) / 50 ,0xFFC107);       // ْX , color
    draw_horizental_line((framebuffer->height ) / 50 ,0xFFC107);   //   Y , color
    draw_horizental_line((49 * framebuffer->height ) / 50 ,0xFFC107);   //   Y , color


    print_centered("BarqOS" , (280 / 4) , 0xFFC107 , 4);
    print_step("Fast like lightning (Maybe)" , 0xFFC107 , 1);
    current_y += (280 - 167);
    print_step("Checking Frambuffer .....\n" , 0xd43100 , 1);
    print_step("(Maybe , If U R here !)" , 0x00d48d , 1);
    print_step("Checking GDT .....\n" , 0xd43100 , 1);
    gdt_init();
    print_step("(Maybe)" , 0x00d48d , 1);
    print_step("Checking IDT .....\n" , 0xd43100 , 1);
    idt_init();
    print_step("(Maybe)" , 0x00d48d , 1);
    print_step("Checking Panic/Interrupt Handler .....\n" , 0xd43100 , 1);
    asm volatile ("int $3"); //Software interrupt
    print_step("Panic handler isn't working !" , 0xd43100 , 1);

    cursor_x = 1;
    cursor_y = 1;
    // We're done, just hang...
    hcf();
}

//////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
// Why ??

// Halt and catch fire function.
void hcf(void) {
    for (;;) {
        asm ("hlt");
    }
}
