#include "game.h"

bool game_init() {
    return true;
}

bool game_draw(uint8_t x, uint8_t y, enum VGA_COLOR color) {
    vga_goto(x, y);
    vga_print_c_color(' ', color, color);

    return true;
}


void game_fill(enum VGA_COLOR color) {
    for (int x = 0; x < 80; x++) {
        for (int y = 0; y < 25; y++) {
            vga_goto(x, y);
            vga_print_c_color(' ', color, color);
        }
    }
}

bool game_draw_shape(char **shape, enum VGA_COLOR color);

bool game_draw_text(uint8_t x, uint8_t y, enum VGA_COLOR color, const char* msg) {
    vga_goto(x, y);
    vga_print_color(msg, color, (enum VGA_COLOR)BLACK);
    return true;
}
