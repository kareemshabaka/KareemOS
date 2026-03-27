#include "string.h"
#include <stdbool.h>

size_t strlen(const char *str)
{
    size_t size = 0;
    while (str[size]) {
        size++;
    }
    return size;
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}