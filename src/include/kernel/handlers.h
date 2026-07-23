#ifndef HANDLERS_H
#define HANDLERS_H

#include <hal.h>

extern int current_y;
void c_divide_by_zero_handler();
void panic(const char *str);

#endif