#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include "stdio.h"

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

#ifdef __cplusplus
    }
#endif


#endif
