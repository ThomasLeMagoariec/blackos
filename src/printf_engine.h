#pragma once
#include <stdarg.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*putc_fn)(char c, void* ctx);

int vprintfn(putc_fn out, void* ctx, const char* fmt, va_list args);
int vsnprintf(char* buf, uint32_t size, const char* fmt, va_list args);
int snprintf(char* buf, uint32_t size, const char* fmt, ...);

#ifdef __cplusplus
}
#endif
