#include "string.h"
#include "mem.h"
#include "types.h"

size_t strlen(const char *str)
{
    size_t size = 0;
    while (str[size]) {
        size++;
    }
    return size;
}

int strcmp(const char *s1, const char *s2)
{
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

char *strtok(char *str, char delim)
{
    static char * last = NULL;

    if (str != NULL) {
        last = str;
    }

    if (last == NULL || *last == '\0') {
        last = NULL;
        return NULL;
    }

    char * start = last;
    while (*last != '\0') {
        if (*last == delim) {
            *last = '\0';
            last++;
            return start;
        }
        last++;
    }

    return start;
    
}

char *strncpy(char *dest, const char *src, size_t n) {
    memcpy(src, dest, n);
    return dest;
}