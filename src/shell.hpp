#ifndef SHELL_H
#define SHELL_H

#include "keyboard.hpp"
#include "string.h"
#include "memdetect.h"
#include "memory.hpp"
#include "games/tetris/tetris.h"


typedef struct {
    bool active;
    size_t capacity;
    size_t count;
    char **words;
} shell_ctx;


void shell_handle_input();
void shell_kb_event(uint8_t scancode);


#endif /* SHELL_H */
