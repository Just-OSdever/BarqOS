#include "handlers.h"
#include "port.h"

void c_divide_by_zero_handler() {
  hal_print("Zero Division Error!", 0xFF0000, 1);
  while (1)
    ;
}

void c_keyboard_handler() {
  uint8_t volatile scancode_num = inb(0x60); // reading the scancode
  static char Scancode_str[4];

  uint_to_string(scancode_num, Scancode_str);
  hal_print_centered("Keboard interrupt", 0xFFC107, 1);
  hal_print_centered("Scancode -> ", 0xFFC107, 1);
  hal_print(Scancode_str, 0xFFC107, 1);
  outb(0x20, 0x20);
}

int ticks = 0;

void c_timer_handler() {
  static char ticks_str[20];
  uint_to_string(ticks, ticks_str);

  hal_print_centered("Ticks : ", 0xFFC107, 1);
  hal_print(ticks_str, 0xFFC107, 1);
  // current_y -= (28 * 1);
  ticks++;
}