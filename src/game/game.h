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
void game_init();
void game_draw(uint8_t x, uint8_t y, enum VGA_COLOR color);
void game_draw_shape(char** shape, enum VGA_COLOR color); // some kind of shape format i'll define later
void game_draw_line(uint8_t startX, uint8_t startY, uint8_t endX, uint8_t endY, enum VGA_COLOR color);
void game_draw_rect(
        uint8_t x1, uint8_t y1,
        uint8_t x2, uint8_t y2,
        uint8_t x3, uint8_t y3,
        uint8_t x4, uint8_t y4,
        enum VGA_COLOR color
);
void game_draw_circle(uint8_t centerX, uint8_t centerY, uint8_t radius, enum VGA_COLOR color);

void game_draw_text(uint8_t x, uint8_t y, enum VGA_COLOR color, const char* msg);
void game_fill(enum VGA_COLOR color);
                                                           
#ifdef __cplusplus
    }
#endif

#endif /* GAME_H */
