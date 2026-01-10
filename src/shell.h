#ifndef SHELL_H
#define SHELL_H


#include "keyboard.h"
#include "string.h"
#include "memdetech.h"

void shell_handle_input();
uint8_t shell_handle_word(char* word, uint8_t len_word);
void shell_handle_echo();

#endif /* SHELL_H */
