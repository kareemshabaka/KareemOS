#ifndef KLIB_H
#define KLIB_H

#include "types.h"

static inline void itohex(uint32_t n, char *buf) {
    char hex_chars[] = "0123456789ABCDEF";
    buf[0] = '0';
    buf[1] = 'x';
    for (int i = 0; i < 8; i++) {
        buf[9 - i] = hex_chars[(n >> (i * 4)) & 0x0F];
    }
    buf[10] = '\0';
}

static inline void itoa(uint32_t n, char *buf) {
    int i = 0;
    if (n == 0) {
        buf[i++] = '0';
    } else {
        while (n > 0) {
            buf[i++] = (char)((n % 10) + '0');
            n /= 10;
        }
    }
    buf[i] = '\0';

    int start = 0;
    int end = i - 1;
    while (start < end) {
        char tmp = buf[start];
        buf[start] = buf[end];
        buf[end] = tmp;
        start++;
        end--;
    }
}

#endif