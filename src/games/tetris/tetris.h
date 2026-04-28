#ifndef TETRIS_H
#define TETRIS_H

#include "../game.h"


#ifdef __cplusplus
    extern "C" {
#endif

typedef struct {
    enum VGA_COLOR bg;
} ctx;


void tetris_init();

#ifdef __cplusplus
    }
#endif


#endif /* TETRIS_H */
