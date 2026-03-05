#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include "stdio.h"
// shell.hpp is C++ code; do not include it here so that C files can
// compile without pulling in C++ headers.

#ifdef __cplusplus
    extern "C" {
#endif


#define MAX_KB_SIZE 256

// kb map
char kb_map_scancode(uint8_t scancode);
void kb_handle_scancode(uint8_t scancode);

// kb buffer
void kb_enqueue_key(uint8_t scancode);
int kb_dequeue_key();
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

#ifdef __cplusplus
    }
#endif


#endif
