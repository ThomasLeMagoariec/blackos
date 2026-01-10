#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include "stdio.h"

#define MAX_KB_SIZE 256

void kb_enqueue_key(uint8_t scancode);
int kb_dequeue_key();
char kb_map_scancode(uint8_t scancode);
void kb_handle_scancode(uint8_t scancode);
char* kb_get_buffer();
void kb_clear_buffer();
uint8_t kb_get_size();

#endif
