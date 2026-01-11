#pragma once
#include "isr.h"
#include "dbg_stdio.h"

#ifdef __cplusplus
    extern "C" {
#endif


typedef void (*IRQHandler)(Registers* regs);

void i686_IRQ_Initialize();
void i686_IRQ_RegisterHandler(int irq, IRQHandler handler);
uint16_t i686_IRQ_GetBuffer();

#ifdef __cplusplus
    }
#endif

