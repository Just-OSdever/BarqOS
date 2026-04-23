#include <panic.h>


void soft_interrupt_handler_c(void* regs){
    (void)regs;
    print_step("(Interrupt Triggered!)" , 0xFFFF00 , 1);
    //hcf();
    return;
}