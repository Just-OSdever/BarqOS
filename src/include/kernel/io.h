#ifndef IO_H
#define IO_H
#include <stdint.h>

static inline void io_wait(void) {
    __asm__ volatile("jmp 1f;1:jmp 1f;1:");
}

#endif