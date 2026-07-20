// #include <limine.h>
// #include <screen.h>
// #include <stddef.h>   // size_t
// #include <stdint.h>

// struct limine_framebuffer *framebuffer = NULL;
// *ptr = 0;

// /* ============================================================
//  *  Init
//  * ============================================================ */
// void framebuffer_init() {
//     if (framebuffer_request.response == NULL ||
//         framebuffer_request.response->framebuffer_count < 1) {
//         hcf(); // Halt and Catch Fire
//     }

//     framebuffer = framebuffer_request.response->framebuffers[0];

//     fb_base         = (uint32_t *)framebuffer->address;
//     fb_pitch_pixels = framebuffer->pitch / 4;
//     fb_width        = framebuffer->width;
//     fb_height       = framebuffer->height;
//     fb_contiguous   = (fb_pitch_pixels == fb_width);
//     fb_total_px     = fb_width * fb_height;
//     *ptr = fb_base;
// }

// /* ============================================================
//  *  Cls func
//  * ============================================================ */

// void cls (uint32_t color) {
//     for (int y = 0; y < fb_height; ++y)
//     {
//         for (int x = 0; x < fb_width; ++x)
//         {
//             // The equation of the addr :  (y * pitch) + x
//             ptr[(y * fb_pitch_pixels) + x] = color;
//         }   
//     }
// }

// /* ============================================================
//  *  Put pixel func
//  * ============================================================ */

// void put_pixel (uint32_t x, uint32_t y, uint32_t color) {
//     // The equation of the addr :  (y * pitch) + x
//     ptr[(y * fb_pitch_pixels) + x] = color;
// }

// /* ============================================================
//  *  Kernel Printing func
//  * ============================================================ */

// void printk(char *text , uint32_t color , uint32_t size) {

// }

// /* ============================================================
//  *  Boot splash
//  * ============================================================ */
// void barqos_boot_splash() {
//     current_y = 60;

//     hal_print("\n\n\n", 0x00BFFF, 1);
//     hal_print("$$$$$$$$     $$$    $$$$$$$$   $$$$$$$    $$$$$$$   $$$$$$$ \n",  0x00BFFF, 1);
//     hal_print("$$     $$   $$ $$   $$     $$ $$     $$  $$     $$ $$     $$\n",  0x33CCFF, 1);
//     hal_print("$$$$$$$$   $$   $$  $$$$$$$$  $$     $$  $$     $$ $$       \n",  0x66FFFF, 1);
//     hal_print("$$     $$ $$$$$$$$$ $$   $$   $$  $$ $$  $$     $$  $$$$$$$ \n",  0x88FFFF, 1);
//     hal_print("$$     $$ $$     $$ $$    $$  $$    $$$  $$     $$        $$\n",  0xAAFFFF, 1);
//     hal_print("$$$$$$$$  $$     $$ $$     $$  $$$$$ $$   $$$$$$$   $$$$$$$ \n\n", 0xFFFFFF, 1);

//     current_y += 25;

//     hal_print("Fast . Modern . Lightweight", 0xC0C0C0, 1);

//     current_y += 77;
// }



#include <limine.h>
#include <screen.h>
#include <stddef.h>     // Required for size_t

// Global variables kept as requested
uint32_t cursor_x = 1;
uint32_t cursor_y = 1;
uint32_t bg_color = 0x0A0F1F ;
int current_y = 0;      // Tracks position under the header
struct limine_framebuffer *framebuffer = NULL;

// --- OPTIMIZATION: Cached properties to avoid struct dereferencing overhead ---
static uint32_t *fb_base = NULL;
static uint32_t fb_pitch_pixels = 0;
static uint32_t fb_width = 0;
static uint32_t fb_height = 0;

void framebuffer_init() {
    // Ensure we got a framebuffer
    if (framebuffer_request.response == NULL ||
        framebuffer_request.response->framebuffer_count < 1) {
        hcf();          // Halt and Catch Fire
    }

    // Fetch the first framebuffer
    framebuffer = framebuffer_request.response->framebuffers[0];
    
    // Cache properties for extreme performance in rendering loops
    fb_base = (uint32_t *)framebuffer->address;
    fb_pitch_pixels = framebuffer->pitch / 4; // Shift pitch from bytes to 32-bit pixels
    fb_width = framebuffer->width;
    fb_height = framebuffer->height;
}

void put_pixel(uint32_t x, uint32_t y, uint32_t color) {
    // Safety and bounds check using fast cached variables
    if (!fb_base || x >= fb_width || y >= fb_height)
        return;

    // Direct memory write using 1D array math
    fb_base[y * fb_pitch_pixels + x] = color;
}

void cls(uint32_t color) {
    if (!fb_base) return;

    // Optimized memory filling row by row
    for (uint32_t y = 0; y < fb_height; y++) {
        uint32_t *row = fb_base + (y * fb_pitch_pixels);
        for (uint32_t x = 0; x < fb_width; x++) {
            row[x] = color;
        }
    }
    bg_color = color;

    cursor_x = 0;
    cursor_y = 0;
}

void draw_vertical_line(uint32_t X, uint32_t color) {
    if (!fb_base || X >= fb_width - 1) return;

    // Direct pointer manipulation avoids put_pixel overhead
    uint32_t *col_ptr = fb_base + X;
    for (uint32_t i = 1; i < fb_height; i++) {
        col_ptr[0] = color;
        col_ptr[1] = color; // Draw 2 pixels wide
        col_ptr += fb_pitch_pixels; // Move directly to the next row
    }
}

void draw_horizental_line(uint32_t Y, uint32_t color) {
    if (!fb_base || Y >= fb_height - 1) return;

    // Optimized memory write for horizontal lines
    uint32_t *row1 = fb_base + (Y * fb_pitch_pixels);
    uint32_t *row2 = fb_base + ((Y + 1) * fb_pitch_pixels);
    
    for (uint32_t i = 1; i < fb_width; i++) {
        row1[i] = color;
        row2[i] = color; // Draw 2 pixels high
    }
}

void framebuffer_scroll(int scale) {
    if (!fb_base) return;

    uint32_t line_height = 24 * scale;
    uint32_t pitch = framebuffer->pitch; // Needs byte-pitch for memcpy
    
    uint8_t *fb_addr = (uint8_t *)framebuffer->address;
    uint8_t *src = fb_addr + (line_height * pitch);
    uint8_t *dst = fb_addr;

    // Shift memory up
    size_t copy_size = (fb_height - line_height) * pitch;
    __builtin_memcpy(dst, src, copy_size);

    // Clear the newly freed space at the bottom
    uint32_t *clear_ptr = (uint32_t *)(fb_addr + (fb_height - line_height) * pitch);
    uint32_t pixels_to_clear = line_height * fb_pitch_pixels;

    for (uint32_t i = 0; i < pixels_to_clear; i++) {
        clear_ptr[i] = bg_color; // Background color
    }

    // Reset cursors dynamically
    cursor_y = fb_height - line_height;
    cursor_x = 1;
    current_y = cursor_y; 
}

void clear_line(uint32_t y, uint32_t height) {
    if (!fb_base) return;

    uint32_t *line_ptr = fb_base + (y * fb_pitch_pixels);
    
    // Fast line clearing
    for (uint32_t h = 0; h < height; h++) {
        for (uint32_t w = 0; w < fb_width; w++) {
            line_ptr[w] = bg_color; 
        }
        line_ptr += fb_pitch_pixels; // Jump to next row
    }
}

void print_char(unsigned char c, uint32_t color, int scale) {
    if (scale %2 == 0) {
        uint32_t new_scale = scale / 2;
        if (!fb_base) return;
        // Handle newline character
        if (c == '\n') {
            cursor_x = 1;
            cursor_y += (24 * new_scale);
            if (cursor_y + (24 * new_scale) >= fb_height) {
                framebuffer_scroll(new_scale);
            }
            return;
        }

        // Retrieve bitmap glyph
        const uint8_t *glyph = &font16x16[(c - 0x20) * 32];            
        for (int y = 0; y < 16; y++) {
            uint8_t byte1 = glyph[y * 2];
            uint8_t byte2 = glyph[y * 2 + 1];

            for (int x = 0; x < 16; x++) {
                uint8_t current_byte = (x < 8) ? byte1 : byte2;
                int bit_pos = (x < 8) ? (7 - x) : (7 - (x - 8));

                // If bit is set, draw the scaled pixel
                if ((current_byte >> bit_pos) & 1) {
                    for (int scale_y = 0; scale_y < new_scale; scale_y++) {
                        for (int scale_x = 0; scale_x < new_scale; scale_x++) {
                            // Inline pixel logic for faster text rendering
                            uint32_t px = cursor_x + (x * new_scale) + scale_x;
                            uint32_t py = cursor_y + (y * new_scale) + scale_y;
                            if (px < fb_width && py < fb_height) {
                                fb_base[py * fb_pitch_pixels + px] = color;
                            }
                        }
                    }
                }
            }
        }

        // Advance cursor
        cursor_x += (16 * new_scale);

        // Auto-wrap and scroll if we hit the screen edge
        if (cursor_x + (16 * new_scale) >= fb_width) {
            cursor_x = 1;
            cursor_y += (24 * new_scale);
            if (cursor_y + (24 * new_scale) >= fb_height) {
                framebuffer_scroll(new_scale);
            }
        }
    }
    else {
        if (!fb_base) return;

        if (c == '\n') {
            cursor_x = 1;
            cursor_y += (14 * scale); // عدلتها عشان تبقى مناسبة لحجم الفونت
            if (cursor_y + (8 * scale) >= fb_height) framebuffer_scroll(scale);
            return;
        }

        if (c < 0x20 || c > 0x7F) return;

        // بنجيب عنوان الحرف من المصفوفة
        const uint8_t *font_ptr = (const uint8_t *)&font8x8_basic[c][0];
        const uint8_t *glyph = font_ptr;
        
        for (int y = 0; y < 8; y++) {
            uint8_t row = glyph[y]; // بايت واحد لكل صف (8 بكسل)

            for (int x = 0; x < 8; x++) {
                // بنتحقق من البت (7 - x) عشان الـ bit 7 هو أول بكسل على الشمال
                if ((row >> (x)) & 1) {
                    // رسم البكسل بحجم الـ scale
                    for (int sy = 0; sy < scale; sy++) {
                        for (int sx = 0; sx < scale; sx++) {
                            uint32_t px = cursor_x + (x * scale) + sx;
                            uint32_t py = cursor_y + (y * scale) + sy;
                            
                            // هنا بنستخدم العنوان مباشرة من الـ Framebuffer
                            // عشان نضمن مفيش غلط في الحسابات
                            if (px < fb_width && py < fb_height) {
                                uint32_t *pixel_ptr = (uint32_t *)((uint8_t *)framebuffer->address + (py * framebuffer->pitch) + (px * 4));
                                *pixel_ptr = color;
                            }
                        }
                    }
                }
            }
        }

        cursor_x += (10 * scale);

        if (cursor_x + (10 * scale) >= fb_width) {
            cursor_x = 1;
            cursor_y += (14 * scale);
            if (cursor_y + (14 * scale) >= fb_height) framebuffer_scroll(scale);
        }
    }
}

void print(const char *str, uint32_t color, int scale) {
    for (int i = 0; str[i] != '\0'; i++) {
        print_char(str[i], color, scale);
    }
}

void print_dec(uint64_t num, uint32_t color, int scale) {
    if (num == 0) {
        print_char('0', color, scale);
        return;
    }
    char buf[32];
    int i = 0;
    while (num > 0) {
        buf[i++] = '0' + (num % 10);
        num /= 10;
    }
    for (int j = i - 1; j >= 0; j--) {
        print_char(buf[j], color, scale);
    }
}

void print_hex(uintptr_t num, uint32_t color, int scale) {
    if (num == 0) {
        print_char('0', color, scale);
        return;
    }
    char buf[32];
    char *hex_chars = "0123456789ABCDEF";
    int i = 0;
    while (num > 0) {
        buf[i++] = hex_chars[num % 16];
        num /= 16;
    }
    for (int j = i - 1; j >= 0; j--) {
        print_char(buf[j], color, scale);
    }
}

void uint_to_string(uint64_t num, char *out_str) {
    int i = 0;
    if (num == 0) {
        out_str[i++] = '0';
    } else {
        while (num > 0) {
            out_str[i++] = (num % 10) + '0';
            num /= 10;
        }
    }
    out_str[i] = '\0';

    // Reverse the string
    for (int j = 0; j < i / 2; j++) {
        char temp = out_str[j];
        out_str[j] = out_str[i - 1 - j];
        out_str[i - 1 - j] = temp;
    }
}

void barqos_boot_splash() {
    current_y = 60;

    // NOTE: Requires hal_print to be defined elsewhere in your kernel
    hal_print("\n\n\n", 0x00BFFF,1);
    hal_print("$$$$$$$$     $$$    $$$$$$$$   $$$$$$$    $$$$$$$   $$$$$$$ \n",  0x00BFFF,1);
    hal_print("$$     $$   $$ $$   $$     $$ $$     $$  $$     $$ $$     $$\n",  0x33CCFF,1);
    hal_print("$$$$$$$$   $$   $$  $$$$$$$$  $$     $$  $$     $$ $$       \n",  0x66FFFF,1);
    hal_print("$$     $$ $$$$$$$$$ $$   $$   $$  $$ $$  $$     $$  $$$$$$$ \n",  0x88FFFF,1);
    hal_print("$$     $$ $$     $$ $$    $$  $$    $$$  $$     $$        $$\n",  0xAAFFFF,1);
    hal_print("$$$$$$$$  $$     $$ $$     $$  $$$$$ $$   $$$$$$$   $$$$$$$ \n\n",0xFFFFFF,1);

    current_y += 25;

    hal_print("Fast . Modern . Lightweight", 0xC0C0C0, 1);

    current_y += 77;
}
