#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <stddef.h>
#include "../stdio.h"
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

#ifdef __cplusplus
    extern "C" {
#endif
bool game_init();
bool game_draw(uint8_t x, uint8_t y, enum VGA_COLOR color);
bool game_draw_shape(char** shape, enum VGA_COLOR color); // some kind of shape format i'll define later
bool game_draw_text(uint8_t x, uint8_t y, enum VGA_COLOR color, const char* msg);
void game_fill(enum VGA_COLOR color);
                                                           
#ifdef __cplusplus
    }
#endif

#endif /* GAME_H */
