#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

#define MAX_KB_SIZE 256

void enqueue_key(uint8_t scancode);
int dequeue_key();

#endif
