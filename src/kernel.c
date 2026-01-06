#include "kernel.h"
#include "irq.h"

void timer(Registers* regs) {
    //printf(".");

    int scancode = dequeue_key();
    if(scancode != -1) {
        char c = map_scancode(scancode);
        printf("%c", c); // your terminal output
    }
}

void irq_keyboard(Registers* regs) {
    handle_scancode(i686_IRQ_GetBuffer());
    //enqueue_key(i686_IRQ_GetBuffer());
}

void kernel_main() {
    HAL_Initialize();


    i686_IRQ_RegisterHandler(0, timer);
    i686_IRQ_RegisterHandler(1, irq_keyboard);
    

    printf("Hello, World!\n");

    dbg_puts("bite\nZeub");

    while (1); // hang
}

