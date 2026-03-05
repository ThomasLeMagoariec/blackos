#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int memcmp(const void* s1, const void* s2, size_t n);
void* memcpy(void* destination, const void* source, size_t size);
int strcmp(const char* s1, const char* s2);
char* strtok(char* str, const char* delim);
char* strdup(char *src);

void* malloc(uint32_t blocks);
void free(void* ptr);

#ifdef __cplusplus
}
#endif

#endif /* STRING_H */
