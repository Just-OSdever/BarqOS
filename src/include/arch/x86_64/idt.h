#ifndef GDT_H
#define GDT_H

#include <stdint.h>
 
void idt_init();
void SET_IDT_ENTRY(int interrupt_num , void* offset , uint16_t segment_selector , uint8_t flags);
extern uint64_t isr_stub_table[];

#endif