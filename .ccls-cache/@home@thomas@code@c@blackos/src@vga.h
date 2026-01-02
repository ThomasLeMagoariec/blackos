#ifndef VGA_H
#define VGA_H

#include <stdint.h>

enum VGA_ERROR {
    SUCCESS,
    FAIL
};


// simple enum with for the vga colors (text mode)
enum VGA_COLOR {
    BLACK =             0,
    BLUE =              1,
    GREEN =             2,
    CYAN =              3,
    RED =               4,
    MAGENTA =           5,
    BROWN =             6,
    LIGHT_GRAY =        7,
    DARK_GRAY =         8,
    LIGHT_BLUE =        9,
    LIGHT_GREEN =       10,
    LIGHT_CYAN =        11,
    LIGHT_RED =         12,
    LIGHT_MAGENTA =     13,
    LIGHT_BROWN =       14,
    WHITE =             15
};


// keep info on the VGA text mode, has to be packed because bits need to be aligned avoid going into unexpected space in ram
struct vga {
    volatile char *addr;
    int cursor;
} __attribute__ ((packed));


void vga_initialize();

static inline uint8_t vga_set_color(enum VGA_COLOR fg, enum VGA_COLOR bg);                      // return color bit

enum VGA_ERROR vga_print(const char* msg);                                               // print using VGA text mode
enum VGA_ERROR vga_print_color(const char* msg, enum VGA_COLOR fg, enum VGA_COLOR bg);   // same as above + color bit
enum VGA_ERROR vga_newline();                                                              // go to new line
enum VGA_ERROR vga_goto(int x, int y);

enum VGA_ERROR vga_print_c_color(const char c, enum VGA_COLOR fg, enum VGA_COLOR bg);

enum VGA_ERROR vga_print_c(const char c);
#endif
