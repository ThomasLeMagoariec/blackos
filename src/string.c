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

/* helper to determine if a character is one of the delimiters */
static int is_delim(char c, const char* delim)
{
    while (*delim) {
        if (c == *delim++)
            return 1;
    }
    return 0;
}

char* strtok(char* str, const char* delim)
{
    static char* next;

    if (str)
        next = str;

    if (next == NULL)
        return NULL;

    /* skip leading delimiters */
    while (*next && is_delim(*next, delim))
        next++;

    if (*next == '\0') {
        next = NULL;
        return NULL;
    }

    char* token_start = next;

    /* advance until delimiter or end */
    while (*next && !is_delim(*next, delim))
        next++;

    if (*next) {
        *next = '\0';
        next++;
    } else {
        next = NULL;
    }

    return token_start;
}
