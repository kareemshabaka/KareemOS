#include "mem.h"

void memcpy(const void *src, void *dst, size_t len)
{
    if (IS_ALIGNED(src, sizeof(uint32_t)) && IS_ALIGNED(dst, uint32_t) &&
        len % sizeof(uint32_t) == 0) {
        const uint32_t *s = (const uint32_t *)src;
        uint32_t       *d = (uint32_t *)dst;
        for (size_t i = 0; i < len / sizeof(uint32_t); ++i) {
            d[i] = s[i];
        }
    } else {
        const char *s = (const char *)src;
        char       *d = (char *)dst;
        for (size_t i = 0; i < len / sizeof(uint8_t); ++i) {
            d[i] = s[i];
        }
    }
}

void *memmove(const void *src, void *dst, size_t len)
{
    unsigned char       *d = (unsigned char *)dst;
    const unsigned char *s = (const unsigned char *)src;

    if (d < s) {
        for (size_t i = 0; i < len; i++) {
            d[i] = s[i];
        }
    } else {
        for (size_t i = len; i > 0; i--) {
            d[i - 1] = s[i - 1];
        }
    }
    return dst;
}

void *memset(void *dst, uint8_t val, size_t len)
{
    uint8_t *ptr = (uint8_t *)dst;
    while (--len > 0) {
        *ptr++ = val;
    }
    return dst;
}

void *memsetw(void *dst, uint16_t val, size_t len)
{
    uint16_t *ptr = (uint16_t *)dst;
    while (--len > 0) {
        *ptr++ = val;
    }
    return dst;
}