#pragma once
#include <stdint.h>

#ifdef __cplusplus
    extern "C" {
#endif

void clrscr();
void putchr(int x, int y, char c);
void putc(char c);
void puts(const char* str);
void printf(const char* fmt, ...);
void print_buffer(const char* msg, const void* buffer, uint32_t count);
void dbg_putc(char c);
void dbg_putstr(const char* str);
void backspace();
void setcursor(int x, int y);

int vsnprintf(char* buf, uint32_t size, const char* fmt, __builtin_va_list args);
int snprintf(char* buf, uint32_t size, const char* fmt, ...);
#ifdef __cplusplus
    }
#endif

