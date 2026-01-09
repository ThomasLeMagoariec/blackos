#include "kernel.h"
#include "irq.h"


void timer(Registers* regs) {

    int scancode = kb_dequeue_key();

    if (scancode != -1) {
        char c = kb_map_scancode(scancode);
        printf("%c", c);
    }


}

void irq_keyboard(Registers* regs) {
    kb_handle_scancode(i686_IRQ_GetBuffer());
}

void kernel_main() {
    HAL_Initialize();


    i686_IRQ_RegisterHandler(0, timer);
    i686_IRQ_RegisterHandler(1, irq_keyboard);
    

    printf("Hello, World!\n");

    printf("> ");

    dbg_puts("bite\nZeub");

    while (1); // hang
}

