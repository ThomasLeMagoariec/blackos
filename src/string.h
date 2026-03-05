 #ifndef STRING_H
 #define STRING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

int memcmp(const void* s1, const void* s2, size_t n);
void* memcpy(void* destination, const void* source, size_t size);

/*
 * Tokenize a string into substrings separated by any of the characters in
 * `delim`.  The first call must provide the string to be tokenized; subsequent
 * calls should pass NULL and the function will continue from the previous
 * position.  This implementation is analogous to the standard library
 * `strtok` and is not thread-safe.
 */
char* strtok(char* str, const char* delim);

#ifdef __cplusplus
}
#endif

#endif /* STRING_H */
