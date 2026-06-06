#include "pic.h"
#include "irq.h"
#include "isr.h"
#include <screen.h>


typedef void (*IRQHandler)(Registers* regs);
IRQHandler g_IRQHandler[16];

void IRQs_Handler(Registers* regs) {
    int irq = regs->int_no - 0x20;
    if (g_IRQHandler[irq] != NULL) {
        g_IRQHandler[irq](regs);
    }
    else {
        char err_str[20];
        uint_to_string(regs->int_no, err_str);
        print("Unhandled IRQ: ", 0xFF0000, 1);
        print(err_str, 0xFF0000, 1);
    }
    PIC_SendEOI(irq);
}

void IRQ_Intialize_PIC() {
    pic_remap();
    g_IRQHandler[0] = c_timer_handler;
    g_IRQHandler[1] = c_keyboard_handler;
}