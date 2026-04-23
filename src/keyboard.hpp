#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include "stdio.h"
#include <stdbool.h>
#include "memory.hpp"

// shell.hpp is C++ code; do not include it here so that C files can
// compile without pulling in C++ headers.

#ifdef __cplusplus
    extern "C" {
#endif


#define MAX_KB_SIZE 256

typedef void (*kb_event)(uint8_t);

typedef struct {
    bool enabled;
    bool shift_pressed;

    char buffer[MAX_KB_SIZE];
    uint8_t bufferPos;

    uint8_t numHandlers;
    kb_event* handlers;
} kb_ctx;

void kb_init();

// kb map
char kb_map_scancode(uint8_t scancode);
void kb_handle_scancode(uint8_t scancode);

// kb buffer
char* kb_get_buffer();
void kb_clear_buffer();
uint8_t kb_get_size();

// kb state
void kb_disable();
void kb_enable();
void kb_toggle();
uint8_t kb_state();

// forward declaration of shell entrypoint so keyboard.c can invoke it without
// including the full C++ shell header.
void shell_handle_input(void);

void register_kbevent(kb_event event);
void kb_main_event(uint8_t scancode);

#ifdef __cplusplus
    }
#endif


#endif
