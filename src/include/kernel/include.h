#ifndef INCLUDE_H
#define INCLUDE_H

#include <apic.h>
#include <gdt.h>
#include <hal.h>
#include <handlers.h>
#include <irq.h>
#include <limine.h>
#include <pmm.h>
#include <pic.h>
#include <port.h>
#include <screen.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

extern void idt_init();
void disable_apic();
extern volatile uint64_t limine_base_revision[];
extern void pmm_init();

#endif