#include "dbg_stdio.h"
#include "io.h"
#include "x86.h"

#include <stdarg.h>
#include <stdbool.h>
#include "printf_engine.h"

static void dbg_raw_putc(char c, void* ctx)
{
    (void)ctx;
    i686_outb(0xE9, c);
}

void dbg_printf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintfn(dbg_raw_putc, NULL, fmt, args);
    va_end(args);
}

void dbg_print_buffer(const char* msg, const void* buffer, uint32_t count)
{
    const uint8_t* u8Buffer = (const uint8_t*)buffer;
    const char g_HexChars_local[] = "0123456789abcdef";
    
    dbg_puts(msg);
    for (uint16_t i = 0; i < count; i++)
    {
        dbg_putc(g_HexChars_local[u8Buffer[i] >> 4]);
        dbg_putc(g_HexChars_local[u8Buffer[i] & 0xF]);
    }
    dbg_puts("\n");
}
