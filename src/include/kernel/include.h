#ifndef INCLUDE_H
#define INCLUDE_H



#include <screen.h>
#include <port.h>
#include <handlers.h>
#include <gdt.h>
#include "irq.h"
#include "pic.h"
#include "apic.h"

extern void idt_init();
void disable_apic();

#endif