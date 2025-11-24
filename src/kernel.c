#include "kernel.h"


void kernel_main() {
    HAL_Initialize();

    const char *msg = "Hello, World! :)";
    const char *line2 = "other line";
    const char *line3 = "bite";

    enum VGA_COLOR fg = RED;
    enum VGA_COLOR bg = BLACK;
    
    vga_initialize();

    //vga_print("Test.");

    puts("Hello, World!");
    /*
    vga_print_color(&vga, msg, fg, bg);
    vga_print(&vga, line2);
    vga_newline(&vga);
    vga_print(&vga, line3);


    vga_goto(&vga, 0, 0);
    vga_print(&vga, "*");

    vga_goto(&vga, 0, 24);
    vga_print(&vga, "*");

    vga_goto(&vga, 79, 0);
    vga_print(&vga, "*");

    vga_goto(&vga, 79, 24);
    vga_print(&vga, "*");

    if (vga_goto(&vga, 80, 30) == (enum VGA_ERROR)FAIL) {
        vga_goto(&vga, 0, 0);
        vga_print_color(&vga, "Memory Overflow", (enum VGA_COLOR)RED, (enum VGA_COLOR)BLACK);
    } else { 
        vga_goto(&vga, 0, 0);
        vga_print_color(&vga, "Should've failed", (enum VGA_COLOR)RED, (enum VGA_COLOR)BLACK);
    }

    __asm("int $0x10");
    */
    while (1); // hang
}

