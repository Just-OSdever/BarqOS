#include "handlers.h"
#include "port.h"

void c_divide_by_zero_handler() {
  hal_print("Zero Division Error!", 0xFF0000, 1);
  while (1)
    ;
}

void lock_printing() {
    asm volatile ("cli");
}

void unlock_printing() {
    asm volatile ("sti");
}

int ticks = 0;

void c_keyboard_handler() { 
    
    uint8_t volatile scancode_num = inb(0x60); 
    static char Scancode_str[4];
    uint_to_string(scancode_num, Scancode_str);
    
    hal_print("Scancode -> ", 0xff00ff, 1);
    hal_print(Scancode_str, 0xff00ff, 1);
    hal_print(" \n", 0xff00ff, 1);
        
    static char ticks_str[20];
    uint_to_string(ticks, ticks_str);
    
    // Save current cursor position to avoid global state corruption
    int temp_y = current_y; 
    current_y = 55;
    
    hal_clear_line(current_y, 24);
    hal_print("Ticks from booting : ", 0xff00ff, 1);
    hal_print(ticks_str, 0xff00ff, 1);
    hal_print("  \n", 0xff00ff, 1);
    
    // Restore original cursor position
    current_y = temp_y; 
    
    outb(0x20, 0x20); 
}

void c_timer_handler() {
    ticks++;
    outb(0x20, 0x20); 
}


void panic(const char *str) {
    hal_cls(0xff0000);
    hal_print(str , 0xffffff , 1);
    while(1);
}