#include "isr.h"

ISRHandler g_ISRHandlers[256];


void i686_ISR_Initialize() {
    i686_ISR_InitializeGates();
    for (int i = 0; i < 256; i++) {
        i686_IDT_EnableGate(i);
    }
}

void __attribute__((cdecl)) i686_ISR_Handler(Registers* regs) {
    if (g_ISRHandlers[regs->interrupt] != NULL) g_ISRHandlers[regs->interrupt](regs);
    else {
        //vga_print("Unhandled interrupt");
    }
}
