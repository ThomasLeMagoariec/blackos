#include "string.h"

int memcmp(const void *s1, const void *s2, size_t n) {
    for (size_t i = 0; i < n; i++) {
        size_t delta = *(unsigned char*)s1++ - *(unsigned char*)s2++;

        if (delta) return delta;
    }

    return 0;
}


void* memcpy( void* destination, const void* source, size_t size ) {
    char* d = (char*)destination;
    const char* s = (const char*)source;
    for (size_t i = 0; i < size; i++) {
        d[i] = s[i];
    }

    return destination;
}
