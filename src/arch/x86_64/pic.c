#include "io.h"
#include "port.h"
#include "pic.h"


void pic_remap() {
    //ICW1 : intalization
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);   //intalized = yes , ready = no
    io_wait();
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    //ICW2 : vector offset
    outb(PIC1_DATA, 0x20); //offset of IRQ0 = 0x20
    io_wait();
    outb(PIC2_DATA, 0x28); //Slave offset = 0x28
    io_wait();
    //ICW3 : connecting master & slave
    outb(PIC1_DATA, 1 << 2); //IRQ2 connected by slave
    io_wait();
    outb(PIC2_DATA, 2);      //no. 2
    io_wait();
    //ICW4 : mode
    outb(PIC1_DATA, ICW4_8086); // = 0x01
    io_wait();
    outb(PIC2_DATA, ICW4_8086); // = 0x01
    io_wait();
    //Unmask
    outb(0x21, 0x00); 
    io_wait();
    outb(0xA1, 0x00); 
}

void PIC_SendEOI(int irq) {
    if (irq >= 8) {
        outb(PIC2_COMMAND , 0x20);
    }
    outb(PIC1_COMMAND , 0x20);
}


uint16_t PIC_GetIRQRequestRegester() {
    outb(PIC1_COMMAND , PIC_READ_IRR);
    outb(PIC2_COMMAND , PIC_READ_IRR);
    return inb(PIC1_COMMAND) | (inb(PIC2_COMMAND) << 8);
}

uint16_t PIC_GetInServiceRegester() {
    outb(PIC1_COMMAND , PIC_READ_ISR);
    outb(PIC2_COMMAND , PIC_READ_ISR);
    return inb(PIC1_COMMAND) | (inb(PIC2_COMMAND) << 8);
}