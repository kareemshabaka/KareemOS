#ifndef KERNEL_H
#define KERNEL_H

#define VERSION_MAJOR 0
#define VERSION_MINOR 1
#define VERSION_DEBUG 0

#include "gdt.h"

void kernel_main(void);
static inline void halt()
{
    asm volatile("hlt");
}

#endif