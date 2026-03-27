#ifndef MEM_H
#define MEM_H

#include "types.h"

#define IS_ALIGNED(ptr, align) (((uintptr_t)(ptr) & ((align) - 1)) == 0)

void  memcpy(const void *src, void *dst, size_t len);
void *memmove(const void *src, void *dst, size_t len);
void *memset(void *dst, uint8_t val, size_t len);
void *memsetw(void *dst, uint16_t val, size_t len);
void  memcmp();

#endif