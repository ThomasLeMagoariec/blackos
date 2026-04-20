#ifndef GAME_H
#define GAME_H

#include <cstdint>
#include <stdbool.h>
#include <stddef.h>
#include "../vga.h"

/*
 * stuf i probably need:
 * init
 * draw_block
 * draw_shape
 * draw_text
 * somekind of physics ??
 * game loop
 * ctx ?
*/

bool game_init();
bool game_draw_block(uint8_t x, uint8_t y, VGA_COLOR color);
bool game_draw_shape(char** shape, VGA_COLOR color); // some kind of shape format i'll define later
bool draw_text(uint8_t x, uint8_t y, VGA_COLOR); // probably just a call to the vga.h function

#endif /* GAME_H */
