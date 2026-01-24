#include "irq.h"
#include "pic.h"
#include "io.h"
#include "stdio.h"
#include <stddef.h>

#define PIC_REMAP_OFFSET        0x20

IRQHandler g_IRQHandlers[16];
uint16_t g_Buff;

uint16_t i686_IRQ_GetBuffer() {
    return g_Buff;
}

void i686_IRQ_Handler(Registers* regs)
{
    g_Buff = i686_PIC_GetBuffer();

    int irq = regs->interrupt - PIC_REMAP_OFFSET;
    
    uint8_t pic_isr = i686_PIC_ReadInServiceRegister();
    uint8_t pic_irr = i686_PIC_ReadIrqRequestRegister();

    if (g_IRQHandlers[irq] != NULL)
    {
        // handle IRQ
        g_IRQHandlers[irq](regs);
    }
    else
    {
        printf("Unhandled IRQ=%d  ISR=%x  IRR=%x  BUFF=%x\n", irq, pic_isr, pic_irr, g_Buff);
    }

    // send EOI
    i686_PIC_SendEndOfInterrupt(irq);
}

void i686_IRQ_Initialize()
{
    dbg_printf("Initialize IRQ...\t");
    i686_PIC_Configure(PIC_REMAP_OFFSET, PIC_REMAP_OFFSET + 8);

    // register ISR handlers for each of the 16 irq lines
    for (int i = 0; i < 16; i++)
        i686_ISR_RegisterHandler(PIC_REMAP_OFFSET + i, i686_IRQ_Handler);

    // enable interrupts
    i686_enableInterrupts();
    dbg_printf("ok\n");
}

void i686_IRQ_RegisterHandler(int irq, IRQHandler handler)
{
    g_IRQHandlers[irq] = handler;
}
