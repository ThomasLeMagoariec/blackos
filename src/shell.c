#include "shell.h"
#include "keyboard.h"

void handle_input() {
    char* buffer = get_kbbuffer();

    for (int i = 0; i < get_kbsize(); i++) {
        printf("%c\n", buffer[i]);
    }

    clear_kbbuffer();
    printf("\n> ");
}
