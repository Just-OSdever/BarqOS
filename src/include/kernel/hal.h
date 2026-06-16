#ifndef HAL_H
#define HAL_H

#include <apic.h>
#include <gdt.h>
#include <idt.h>
#include <irq.h>
#include <isr.h>
#include <limine.h>
#include <pic.h>
#include <screen.h>
#include <stdint.h>

void hal_init();
void hal_print(const char *str, uint32_t color, int scale);
void hal_print_centered(char *text, uint32_t color, int scale);
void hal_print_dec(uint64_t num, uint32_t color, int scale);
void hal_print_hex(uint64_t num, uint32_t color, int scale);
void hal_cls(uint32_t color);

#endif