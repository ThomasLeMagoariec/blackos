#pragma once
#include "stdint.h"

void __attribute__((cdecl)) x86_outb(uint16_t port, uint8_t value);
void __attribute__((cdecl)) x86_div64_32(uint64_t dividend, uint32_t divisor, uint64_t* quotientOut, uint32_t* remainderOut);

void __attribute__((cdecl)) x86_Video_WriteCharTeletype(char c, uint8_t page);
