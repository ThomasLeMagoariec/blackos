#include "tetris.h"

void tetris_init() {
    game_init();
    game_draw(10, 10, (enum VGA_COLOR)RED);
    game_fill((enum VGA_COLOR)MAGENTA);
    game_draw_text(30, 5, (enum VGA_COLOR)RED, "Test.");

    game_draw_rect(5, 5, 15, 5, 15, 15, 5, 15, (enum VGA_COLOR)BLUE);

    game_draw_circle(40, 20, 3, (enum VGA_COLOR)BLUE);
    game_draw_circle(46, 20, 3, (enum VGA_COLOR)BLUE);
    game_draw_rect(42, 7, 44, 7, 44, 20, 42, 20, (enum VGA_COLOR)BLUE);
}


