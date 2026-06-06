#ifndef IRQ_H
#define IRQ_H

void IRQ_Intialize_PIC();
extern void c_timer_handler();
extern void c_keyboard_handler();

#endif