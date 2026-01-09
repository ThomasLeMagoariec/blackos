#ifndef SHELL_H
#define SHELL_H


#include "keyboard.h"
#include "string.h"

void handle_input();
uint8_t handle_word(char* word, uint8_t len_word);
void handle_echo();

#endif /* SHELL_H */
