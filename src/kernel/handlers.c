#include "handlers.h"
#include "port.h"

void c_divide_by_zero_handler() {
    print("Zero Division Error!", 0xFF0000, 1);
    while(1);
}

void c_keyboard_handler() {
    uint8_t volatile scancode_num = inb(0x60);  //reading the scancode
    static char Scancode_str[4];

    uint_to_string(scancode_num , Scancode_str);
    print_step("Keboard interrupt" , 0xFFC107 , 1);
    print_step("Scancode -> " , 0xFFC107 , 1);
    print(Scancode_str , 0xFFC107 , 1);
    outb(0x20, 0x20);
}

int ticks = 0;

void c_timer_handler() {
    static char ticks_str[20];
    uint_to_string(ticks, ticks_str);
    
    print_step("Ticks : ", 0xFFC107, 1);
    print(ticks_str, 0xFFC107, 1);
    //current_y -= (28 * 1);
    ticks++;
}