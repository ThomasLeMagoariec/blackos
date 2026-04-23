#include "kernel.hpp"
#include "game/game.h"
#include "irq.h"
#include "keyboard.hpp"
#include "memory.hpp"
#include "shell.hpp"
#include "vga.h"

void timer(Registers* regs) {

}

void irq_keyboard(Registers* regs) {
    kb_handle_scancode(i686_IRQ_GetBuffer());
}

void kernel_main(uint32_t magic, struct multiboot_info* mbi) {
    HAL_Initialize();
    mem_ctx ctx = mem_Initialize(mbi);

    int *ptr = (int *)malloc(20);

    for (int i = 0; i < 20; ++i)
        ptr[i] = i;
    printf("first element: %d\n", ptr[0]);

    ptr = (int*)realloc(ptr, 40);
    if (ptr) {
        for (int i = 20; i < 40; ++i)
            ptr[i] = i;
        printf("after grow element 30: %d\n", ptr[30]);
    }

    free(ptr);


    /*
    BuddyAllocator* alloc = new BuddyAllocator();
    //BuddyAllocator::Initialize(4096, new_regions, region_count);
    alloc->Initialize(4096, new_regions, region_count);
    */


    kb_init();

    i686_IRQ_RegisterHandler(0, timer);
    i686_IRQ_RegisterHandler(1, irq_keyboard);

    register_kbevent(shell_kb_event);
    register_kbevent(kb_main_event);
    

    vga_initialize();

    printf("Hello, World!\n");

    printf("> ");
    vga_print_color("*", RED, RED);


    /*
    game_init();
    game_draw(10, 10, (enum VGA_COLOR)RED);
    game_fill((enum VGA_COLOR)MAGENTA);
    game_draw_text(30, 5, (enum VGA_COLOR)RED, "Test.");

    game_draw_rect(5, 5, 15, 5, 15, 15, 5, 15, (enum VGA_COLOR)BLUE);

    game_draw_circle(40, 20, 3, (enum VGA_COLOR)BLUE);
    game_draw_circle(46, 20, 3, (enum VGA_COLOR)BLUE);
    game_draw_rect(42, 7, 44, 7, 44, 20, 42, 20, (enum VGA_COLOR)BLUE);
    */

    while (1); // hang
}

