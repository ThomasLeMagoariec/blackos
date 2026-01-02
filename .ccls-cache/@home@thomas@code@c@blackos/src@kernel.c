#include "kernel.h"
#include "irq.h"

void timer(Registers* regs) {
    printf(".");

    int code;
    while (code != -1) {
        code = dequeue_key();
        if (code != -1) printf("%x", code);
        else break;
    }
}

void irq_keyboard(Registers* regs) {
    enqueue_key(i686_IRQ_GetBuffer());
}

void kernel_main() {
    HAL_Initialize();


    i686_IRQ_RegisterHandler(0, timer);
    i686_IRQ_RegisterHandler(1, irq_keyboard);
    
    printf("Hello, World!\n");

    dbg_puts("bite\nZeub");

    while (1); // hang
}

