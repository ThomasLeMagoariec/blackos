#include "vga.h"

struct vga g_vga;

void vga_initialize() {
    g_vga.addr = (volatile char*)0xB8000;
    g_vga.cursor = 0;
}


static inline uint8_t vga_set_color(enum VGA_COLOR fg, enum VGA_COLOR bg) {
    return fg | bg << 4;
}

enum VGA_ERROR vga_print(const char* msg) { 
    return vga_print_color(msg, (enum VGA_COLOR)WHITE, (enum VGA_COLOR)BLACK);
}

enum VGA_ERROR vga_print_color(const char* msg, enum VGA_COLOR fg, enum VGA_COLOR bg) {

    int s = 0;

    for (int i = 0; msg[i] != '\0'; i++) {
        if (g_vga.cursor >= 2000) return (enum VGA_ERROR)FAIL;

        g_vga.addr[(g_vga.cursor + i) * 2] = msg[i];                      // 2 bits per char
        g_vga.addr[(i+g_vga.cursor) * 2 + 1 ] = vga_set_color(fg, bg);    // + 1 bit for color
        s++;
    }

    g_vga.cursor += s;

    return (enum VGA_ERROR)SUCCESS;
}


enum VGA_ERROR vga_print_c(const char c) {
    vga_print_c_color(c, (enum VGA_COLOR)WHITE, (enum VGA_COLOR)BLACK);
    return (enum VGA_ERROR)SUCCESS;
}

enum VGA_ERROR vga_print_c_color(const char c, enum VGA_COLOR fg, enum VGA_COLOR bg) {
    g_vga.addr[(g_vga.cursor) * 2] = c;                      // 2 bits per char
    g_vga.addr[(g_vga.cursor) * 2 + 1 ] = vga_set_color(fg, bg);    // + 1 bit for color
    
    return (enum VGA_ERROR)SUCCESS;
}

enum VGA_ERROR vga_newline() {
    if (g_vga.cursor > 1920) return (enum VGA_ERROR)FAIL;
    g_vga.cursor = (g_vga.cursor / 80 + 1) * 80;      // each line is 80 chars wide
    
    return (enum VGA_ERROR)SUCCESS;
}

enum VGA_ERROR vga_goto(int x, int y) {
    if (x >= 80 || y >= 25) return (enum VGA_ERROR)FAIL;

    g_vga.cursor = 80 * y + x;

    return (enum VGA_ERROR)SUCCESS;
}
