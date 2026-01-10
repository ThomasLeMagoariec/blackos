#include "shell.h"
#include "keyboard.h"

char g_CommandArgsBuffer[16][256];
char* g_CurrentCommand[16];
uint8_t g_NumWords;

void shell_handle_echo() {
    for (int i = 1; i < g_NumWords; i++) {
        if (i == g_NumWords - 1)  printf("%s", g_CurrentCommand[i]);
        else printf("%s ", g_CurrentCommand[i]);
    }
}

void shell_handle_kbtest() {
    printf("disabling keyboard ...\t");
    kb_disable();
    printf("%s", kb_state() ? "ENABLED\n" : "DISABLED\n");
    printf("enabling keyboard ... \t");
    kb_enable();
    printf("%s", kb_state() ? "ENABLED" : "DISABLED");
}

void shell_add_command_word(const char* word_str, uint8_t len_word) {
    if (g_NumWords < 16) {
        uint8_t copy_len = (len_word < 255) ? len_word : 255;
        memcpy(g_CommandArgsBuffer[g_NumWords], word_str, copy_len);

        g_CommandArgsBuffer[g_NumWords][copy_len] = '\0';

        g_CurrentCommand[g_NumWords] = g_CommandArgsBuffer[g_NumWords];
        g_NumWords++;
    }
}

void shell_handle_input() {
    char* buffer = kb_get_buffer();
    char word[256];
    uint8_t len_word = 0;
    
    g_NumWords = 0;
    
    for (uint8_t i = 0; i < kb_get_size(); i++) {
        if (buffer[i] != ' ' && buffer[i] != '\n') {
            if (len_word < 255) word[len_word++] = buffer[i];
        } else {
            if (len_word > 0) {
                word[len_word] = '\0';
                shell_add_command_word(word, len_word);
                len_word = 0;
            }
        }
    }

    if (len_word > 0) {
        word[len_word] = '\0';
        shell_add_command_word(word, len_word);
    }


    if (memcmp(g_CurrentCommand[0], "echo", 4) == 0) {
        shell_handle_echo();
    } else if (memcmp(g_CurrentCommand[0], "memmap", 6) == 0){
        print_memmap();
    } else if (memcmp(g_CurrentCommand[0], "kbtest", 6) == 0){
        shell_handle_kbtest();
    } else {
        printf("UNDEFINED\n");
    }


    kb_clear_buffer();
    printf("\n> ");
}

