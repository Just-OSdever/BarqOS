#include <limine.h>
#include <screen.h>

uint32_t cursor_x = 1;
uint32_t cursor_y = 1;
struct limine_framebuffer *framebuffer = NULL; // Forward declaration

void cls(uint32_t color) {
  // 1- safty check
  if (framebuffer == NULL)
    return;
  framebuffer = framebuffer_request.response->framebuffers[0];
  for (uint32_t y = 0; y < framebuffer->height; y++) {
    uint32_t *row = (uint32_t *)((uint8_t *)framebuffer->address +
                                 (y * framebuffer->pitch));

    for (uint32_t x = 0; x < framebuffer->width; x++) {
      row[x] = color;
    }
  }
}

void put_pixel(uint32_t x, uint32_t y, uint32_t color) {
  if (framebuffer == NULL)
    return;

  if (x >= framebuffer->width || y >= framebuffer->height)
    return;

  volatile uint32_t *fb_ptr = framebuffer->address;
  fb_ptr[y * (framebuffer->pitch / 4) + x] = color;
}

void framebuffer_init() {
  // Ensure we got a framebuffer.
  if (framebuffer_request.response == NULL ||
      framebuffer_request.response->framebuffer_count < 1) {
    hcf();
  }

  // Fetch the first framebuffer.  (There could be more than one)
  framebuffer = framebuffer_request.response->framebuffers[0];
}

void draw_vertical_line(uint32_t X, uint32_t color) {
  if (framebuffer == NULL)
    return;
  for (uint64_t i = 1; i < framebuffer->height; i++) {
    put_pixel(X, i, color);
    put_pixel((X + 1), i, color); // 2 Pixel size
  }
}

void draw_horizental_line(uint32_t Y, uint32_t color) {
  if (framebuffer == NULL)
    return;
  for (uint64_t i = 1; i < framebuffer->width; i++) {
    put_pixel(i, Y, color);
    put_pixel(i, Y + 1, color); // 2 Pixel size
  }
}

void framebuffer_scroll(int scale) {
  if (framebuffer == NULL)
    return;

  uint32_t line_height = 24 * scale;
  uint32_t pitch = framebuffer->pitch;
  uint8_t *fb_addr = (uint8_t *)framebuffer->address;

  uint32_t copy_height = framebuffer->height - line_height;
  uint64_t total_bytes_to_copy = (uint64_t)copy_height * pitch;
  uint8_t *src = fb_addr + ((uint64_t)line_height * pitch);
  uint8_t *dst = fb_addr;

  for (uint64_t i = 0; i < total_bytes_to_copy; i++) {
    dst[i] = src[i];
  }

  uint64_t clear_offset = (uint64_t)(framebuffer->height - line_height) * pitch;
  uint32_t *clear_ptr = (uint32_t *)(fb_addr + clear_offset);
  uint32_t pixels_to_clear = line_height * (pitch / 4);

  for (uint32_t i = 0; i < pixels_to_clear; i++) {
    clear_ptr[i] = 0x0A0F1F;
  }

  cursor_y = framebuffer->height - line_height;
  cursor_x = 1;
}

void print_char(char c, uint32_t color, int scale) {
  if (framebuffer == NULL)
    return;

  if (c == '\n') {
    cursor_x = 1;
    cursor_y += (24 * scale);
    if (cursor_y + (24 * scale) >= framebuffer->height) {
      framebuffer_scroll(scale);
    }
    return;
  }

  const uint8_t *glyph = &font16x16[(c - 0x20) * 32];
  for (int y = 0; y < 16; y++) {
    uint8_t byte1 = glyph[y * 2];
    uint8_t byte2 = glyph[y * 2 + 1];

    for (int x = 0; x < 16; x++) {
      uint8_t current_byte = (x < 8) ? byte1 : byte2;
      int bit_pos = (x < 8) ? (7 - x) : (7 - (x - 8));

      if ((current_byte >> bit_pos) & 1) {
        for (int scale_y = 0; scale_y < scale; scale_y++) {
          for (int scale_x = 0; scale_x < scale; scale_x++) {
            put_pixel(cursor_x + (x * scale) + scale_x,
                      cursor_y + (y * scale) + scale_y, color);
          }
        }
      }
    }
  }

  cursor_x += (16 * scale);

  if (cursor_x + (16 * scale) >= framebuffer->width) {
    cursor_x = 1;
    cursor_y += (24 * scale);
    if (cursor_y + (24 * scale) >= framebuffer->height) {
      framebuffer_scroll(scale);
    }
  }
}

void print(const char *str, uint32_t color, int scale) {
  for (int i = 0; str[i] != '\0'; i++) {
    print_char(str[i], color, scale);
  }
}

int current_y = 167; // under the header

void print_step(char *text, uint32_t color, int scale) {
  int char_width = 16 * scale;
  int str_len = 0;

  while (text[str_len] != '\0')
    str_len++;

  int text_width = str_len * char_width;

  int x = 0;

  if (text_width < framebuffer->width)
    x = (framebuffer->width - text_width) / 2;

  cursor_x = x;
  cursor_y = current_y;

  print(text, color, scale);

  current_y += (28 * scale);
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

  for (int j = 0; j < i / 2; j++) {
    char temp = out_str[j];
    out_str[j] = out_str[i - 1 - j];
    out_str[i - 1 - j] = temp;
  }
}

void barqos_boot_splash() {

  current_y = 60;

  hal_print_centered(
      "$$$$$$$$     $$$    $$$$$$$$   $$$$$$$    $$$$$$$   $$$$$$$ ", 0x00BFFF,
      1);
  hal_print_centered(
      "$$     $$   $$ $$   $$     $$ $$     $$  $$     $$ $$     $$", 0x33CCFF,
      1);
  hal_print_centered(
      "$$$$$$$$   $$   $$  $$$$$$$$  $$     $$  $$     $$ $$       ", 0x66FFFF,
      1);
  hal_print_centered(
      "$$     $$ $$$$$$$$$ $$   $$   $$  $$ $$  $$     $$  $$$$$$$ ", 0x88FFFF,
      1);
  hal_print_centered(
      "$$     $$ $$     $$ $$    $$  $$    $$$  $$     $$        $$", 0xAAFFFF,
      1);
  hal_print_centered(
      "$$$$$$$$  $$     $$ $$     $$  $$$$$ $$   $$$$$$$   $$$$$$$ ", 0xFFFFFF,
      1);

  current_y += 25;

  hal_print_centered("Fast . Modern . Lightweight", 0xC0C0C0, 1);

  current_y += 77;
}