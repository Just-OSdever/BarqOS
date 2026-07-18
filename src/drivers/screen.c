#include <limine.h>
#include <screen.h>
#include <stddef.h>   // size_t
#include <stdint.h>

/* ============================================================
 *  Console / cursor state
 * ============================================================ */
uint32_t cursor_x   = 1;
uint32_t cursor_y   = 1;
uint32_t bg_color   = 0x0A0F1F;
int      current_y  = 0;

struct limine_framebuffer *framebuffer = NULL;

/* ============================================================
 *  Cached framebuffer properties — avoids struct dereferencing
 *  in every hot-path call.
 * ============================================================ */
static uint32_t *fb_base         = NULL;
static uint32_t  fb_pitch_pixels = 0;   // pitch in 32-bit pixels, not bytes
static uint32_t  fb_width        = 0;
static uint32_t  fb_height       = 0;
static uint32_t  fb_total_px     = 0;   // width*height, valid only when pitch==width
static int       fb_contiguous   = 0;   // 1 if pitch_pixels == width (no row padding)

#define GLYPH_W        16
#define GLYPH_H        16
#define LINE_HEIGHT_PX 24   // vertical advance per text line (unscaled)

/* ============================================================
 *  Init
 * ============================================================ */
void framebuffer_init() {
    if (framebuffer_request.response == NULL ||
        framebuffer_request.response->framebuffer_count < 1) {
        hcf(); // Halt and Catch Fire
    }

    framebuffer = framebuffer_request.response->framebuffers[0];

    fb_base         = (uint32_t *)framebuffer->address;
    fb_pitch_pixels = framebuffer->pitch / 4;
    fb_width        = framebuffer->width;
    fb_height       = framebuffer->height;
    fb_contiguous   = (fb_pitch_pixels == fb_width);
    fb_total_px     = fb_width * fb_height;

    /* NOTE: if scrolling/clears are still slow after this rewrite, the
     * remaining cost is almost certainly the memory type of the FB
     * mapping. Make sure the page tables map this physical range as
     * Write-Combining (PAT/PCD/PWT), not Uncacheable or plain WB.
     * A UC framebuffer will make even a single memset() crawl,
     * regardless of how tight this code is. */
}

/* ============================================================
 *  Low-level fast fill helper
 *  Fills `count` consecutive 32-bit pixels starting at `dst`
 *  with `color`, unrolled 8-wide so the compiler can pack it
 *  into wide stores instead of one store per loop iteration.
 * ============================================================ */
static inline void fast_fill32(uint32_t *dst, uint32_t color, uint32_t count) {
    uint32_t i = 0;
    uint32_t unrolled = count & ~7u; // largest multiple of 8 <= count

    for (; i < unrolled; i += 8) {
        dst[i]     = color;
        dst[i + 1] = color;
        dst[i + 2] = color;
        dst[i + 3] = color;
        dst[i + 4] = color;
        dst[i + 5] = color;
        dst[i + 6] = color;
        dst[i + 7] = color;
    }
    for (; i < count; i++) {
        dst[i] = color;
    }
}

/* ============================================================
 *  Pixel primitives
 * ============================================================ */
void put_pixel(uint32_t x, uint32_t y, uint32_t color) {
    if (!fb_base || x >= fb_width || y >= fb_height)
        return;
    fb_base[y * fb_pitch_pixels + x] = color;
}

void cls(uint32_t color) {
    if (!fb_base) return;

    if (fb_contiguous) {
        // Whole framebuffer is one contiguous block — single fast fill.
        fast_fill32(fb_base, color, fb_total_px);
    } else {
        // Padded rows — fill each row's visible pixels only.
        uint32_t *row = fb_base;
        for (uint32_t y = 0; y < fb_height; y++) {
            fast_fill32(row, color, fb_width);
            row += fb_pitch_pixels;
        }
    }
    bg_color = color;
}

void clear_line(uint32_t y, uint32_t height) {
    if (!fb_base) return;

    uint32_t max_h = (y + height > fb_height) ? (fb_height - y) : height;
    uint32_t *row = fb_base + (size_t)y * fb_pitch_pixels;

    for (uint32_t h = 0; h < max_h; h++) {
        fast_fill32(row, bg_color, fb_width);
        row += fb_pitch_pixels;
    }
}

void draw_vertical_line(uint32_t X, uint32_t color) {
    if (!fb_base || X >= fb_width - 1) return;

    uint32_t *col_ptr = fb_base + X;
    for (uint32_t i = 1; i < fb_height; i++) {
        col_ptr[0] = color;
        col_ptr[1] = color; // 2px wide
        col_ptr += fb_pitch_pixels;
    }
}

void draw_horizental_line(uint32_t Y, uint32_t color) {
    if (!fb_base || Y >= fb_height - 1) return;

    uint32_t *row1 = fb_base + (size_t)Y * fb_pitch_pixels;
    uint32_t *row2 = row1 + fb_pitch_pixels;

    fast_fill32(row1 + 1, color, fb_width - 1);
    fast_fill32(row2 + 1, color, fb_width - 1);
}

/* ============================================================
 *  Scrolling
 *  memcpy is already the fastest correct approach here — the
 *  original bottleneck (if any) is the FB memory type, not this
 *  code. The clear-after-scroll now uses fast_fill32 instead of
 *  a naive per-pixel loop.
 * ============================================================ */
void framebuffer_scroll(int scale) {
    if (!fb_base) return;

    uint32_t line_height = LINE_HEIGHT_PX * (uint32_t)scale;
    if (line_height >= fb_height) return; // nothing sane to scroll

    uint32_t pitch_bytes = framebuffer->pitch;

    uint8_t *fb_addr = (uint8_t *)framebuffer->address;
    uint8_t *src = fb_addr + (size_t)line_height * pitch_bytes;
    uint8_t *dst = fb_addr;

    size_t copy_size = (size_t)(fb_height - line_height) * pitch_bytes;
    __builtin_memcpy(dst, src, copy_size);

    uint32_t *clear_ptr = (uint32_t *)(fb_addr + (size_t)(fb_height - line_height) * pitch_bytes);
    uint32_t pixels_to_clear = line_height * fb_pitch_pixels;
    fast_fill32(clear_ptr, bg_color, pixels_to_clear);

    cursor_y   = fb_height - line_height;
    cursor_x   = 1;
    current_y  = cursor_y;
}

/* ============================================================
 *  Text rendering
 *  Rewritten inner loop: row pointer + x pointer are hoisted out
 *  so each sub-pixel write is a plain store, not a recomputed
 *  `py * pitch + px` multiply-add.
 * ============================================================ */
static inline void advance_cursor_and_wrap(int scale) {
    cursor_x += (GLYPH_W * scale);
    if (cursor_x + (GLYPH_W * scale) >= fb_width) {
        cursor_x = 1;
        cursor_y += (LINE_HEIGHT_PX * scale);
        if (cursor_y + (LINE_HEIGHT_PX * scale) >= fb_height) {
            framebuffer_scroll(scale);
        }
    }
}

void print_char(char c, uint32_t color, int scale) {
    if (!fb_base) return;

    if (c == '\n') {
        cursor_x = 1;
        cursor_y += (LINE_HEIGHT_PX * scale);
        if (cursor_y + (LINE_HEIGHT_PX * scale) >= fb_height) {
            framebuffer_scroll(scale);
        }
        return;
    }

    const uint8_t *glyph = &font16x16[(c - 0x20) * 32];

    for (int y = 0; y < GLYPH_H; y++) {
        uint8_t byte1 = glyph[y * 2];
        uint8_t byte2 = glyph[y * 2 + 1];

        // Skip fully-empty glyph rows entirely (common for whitespace-heavy glyphs).
        if (byte1 == 0 && byte2 == 0) continue;

        for (int scale_y = 0; scale_y < scale; scale_y++) {
            uint32_t py = cursor_y + (uint32_t)(y * scale) + (uint32_t)scale_y;
            if (py >= fb_height) continue;

            uint32_t *row = fb_base + (size_t)py * fb_pitch_pixels;

            for (int x = 0; x < GLYPH_W; x++) {
                uint8_t cur_byte = (x < 8) ? byte1 : byte2;
                int bit_pos = (x < 8) ? (7 - x) : (7 - (x - 8));
                if (!((cur_byte >> bit_pos) & 1)) continue;

                uint32_t px_base = cursor_x + (uint32_t)(x * scale);

                for (int scale_x = 0; scale_x < scale; scale_x++) {
                    uint32_t px = px_base + (uint32_t)scale_x;
                    if (px < fb_width) row[px] = color;
                }
            }
        }
    }

    advance_cursor_and_wrap(scale);
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
    char buf[20]; // max uint64 digits = 20
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
    static const char hex_chars[] = "0123456789ABCDEF";
    char buf[16]; // max 64-bit hex digits
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
            out_str[i++] = (char)((num % 10) + '0');
            num /= 10;
        }
    }
    out_str[i] = '\0';

    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char tmp = out_str[j];
        out_str[j] = out_str[k];
        out_str[k] = tmp;
    }
}

/* ============================================================
 *  Boot splash
 * ============================================================ */
void barqos_boot_splash() {
    current_y = 60;

    hal_print("\n\n\n", 0x00BFFF, 1);
    hal_print("$$$$$$$$     $$$    $$$$$$$$   $$$$$$$    $$$$$$$   $$$$$$$ \n",  0x00BFFF, 1);
    hal_print("$$     $$   $$ $$   $$     $$ $$     $$  $$     $$ $$     $$\n",  0x33CCFF, 1);
    hal_print("$$$$$$$$   $$   $$  $$$$$$$$  $$     $$  $$     $$ $$       \n",  0x66FFFF, 1);
    hal_print("$$     $$ $$$$$$$$$ $$   $$   $$  $$ $$  $$     $$  $$$$$$$ \n",  0x88FFFF, 1);
    hal_print("$$     $$ $$     $$ $$    $$  $$    $$$  $$     $$        $$\n",  0xAAFFFF, 1);
    hal_print("$$$$$$$$  $$     $$ $$     $$  $$$$$ $$   $$$$$$$   $$$$$$$ \n\n", 0xFFFFFF, 1);

    current_y += 25;

    hal_print("Fast . Modern . Lightweight", 0xC0C0C0, 1);

    current_y += 77;
}
