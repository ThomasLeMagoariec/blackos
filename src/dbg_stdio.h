#pragma once
#include <stdint.h>

void dbg_clrscr();
void dbg_putc(char c);
void dbg_puts(const char* str);
void dbg_printf(const char* fmt, ...);
void dbg_print_buffer(const char* msg, const void* buffer, uint32_t count);
void dbg_putc(char c);
void dbg_putstr(const char* str);
