#ifndef SHELL_H
#define SHELL_H

#include "keyboard.h"
#include "string.h"
#include "memdetect.h"
#include "memory.hpp"


typedef struct {
    size_t capacity;
    size_t count;
    char **words;
} shell_ctx;


void shell_handle_input();

#endif /* SHELL_H */
