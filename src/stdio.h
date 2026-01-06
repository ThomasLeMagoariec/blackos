#pragma once
#include <stdint.h>

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
