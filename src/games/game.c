#include "game.h"

void game_init() {
    vga_initialize();
}

void game_draw(uint8_t x, uint8_t y, enum VGA_COLOR color) {
    vga_goto(x, y);
    vga_print_c_color(' ', color, color);

}


void game_fill(enum VGA_COLOR color) {
    for (int x = 0; x < 80; x++) {
        for (int y = 0; y < 25; y++) {
            vga_goto(x, y);
            vga_print_c_color(' ', color, color);
        }
    }
}

void game_draw_shape(char **shape, enum VGA_COLOR color);

void game_draw_line(uint8_t startX, uint8_t startY, uint8_t endX, uint8_t endY, enum VGA_COLOR color) {
    if (startX > endX) {
        int tmp = startX;
        startX = endX;
        endX = tmp;
    }

    if (startY > endY) {
        int tmp = startY;
        startY = endY;
        endY = tmp;
    }

    if (startX == endX) {
        for (int y = startY; y <= endY; y++) {
            game_draw(startX, y, color);
        }
    }

    if (startY == endY) {
        for (int x = startX; x <= endX; x++) {
            game_draw(x, startY, color);
        }
    }

    int dx = endX - startX;
    int dy = endY - startY;
    int D = 2*dy - dx;
    int y = startY;

    for (int x = startX; x < endX; x++) {
        game_draw(x, y, color);

        if (D > 0) {
            y++;
            D = D + (2 * (dy - dx));
        } else {
            D = D + (2 * dy);
        }
    }
}

void game_draw_rect(
        uint8_t x1, uint8_t y1,
        uint8_t x2, uint8_t y2,
        uint8_t x3, uint8_t y3,
        uint8_t x4, uint8_t y4,
        enum VGA_COLOR color
) {
    game_draw_line(x1, y1, x2, y2, color);
    game_draw_line(x2, y2, x3, y3, color);
    game_draw_line(x3, y3, x4, y4, color);
    game_draw_line(x4, y4, x1, y1, color);
}


void game_draw_circle(uint8_t centerX, uint8_t centerY, uint8_t radius, enum VGA_COLOR color) {
    int f = 1 - radius;
    int ddF_x = 0;
    int ddF_y = -2 * radius;
    int x = 0;
    int y = radius;

    game_draw(centerX, centerY + radius, color);
    game_draw(centerX, centerY - radius, color);
    game_draw(centerX + radius, centerY, color);
    game_draw(centerX - radius, centerY, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }

        x++;
        ddF_x += 2;
        f += ddF_x + 1;

        game_draw(centerX + x, centerY + y, color);
        game_draw(centerX - x, centerY + y, color);
        game_draw(centerX + x, centerY - y, color);
        game_draw(centerX - x, centerY - y, color);
        game_draw(centerX + y, centerY + x, color);
        game_draw(centerX - y, centerY + x, color);
        game_draw(centerX + y, centerY - x, color);
        game_draw(centerX - y, centerY - x, color);
    }
}

void game_draw_text(uint8_t x, uint8_t y, enum VGA_COLOR color, const char* msg) {
    vga_goto(x, y);
    vga_print_color(msg, color, (enum VGA_COLOR)BLACK);
}
