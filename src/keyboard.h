#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include "stdio.h"

#define MAX_KB_SIZE 256

void enqueue_key(uint8_t scancode);
int dequeue_key();
char map_scancode(uint8_t scancode);
void handle_scancode(uint8_t scancode);
char* get_kbbuffer();
void clear_kbbuffer();
uint8_t get_kbsize();

#endif
