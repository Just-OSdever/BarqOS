#ifndef APIC_H
#define APIC_H

#include <hal.h>
#include <irq.h>
#include <limine.h>
#include <msr.h>
#include <pic.h>
#include <port.h>
#include <screen.h>
#include <stdint.h>

extern uint32_t detect_apic();
extern uint64_t cpu_get_apic_base();
extern volatile struct limine_hhdm_request hhdm_request;
void enable_apic();
void APIC_Intialize();
extern struct limine_framebuffer *framebuffer; // Forward declaration
void disable_apic();

#endif