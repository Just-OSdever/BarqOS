#ifndef ALLOCATOR_TEST_H
#define ALLOCATOR_TEST_H

#include <stdint.h>
#include <stddef.h>
#include <strings.h>
#include <pmm.h>
#include <handlers.h>
#include <hal.h>

void allocator_test(void);

#define ASSERT(cond, msg) \
    do {                  \
        if (!(cond))       \
            panic(msg);    \
    } while (0)


#endif