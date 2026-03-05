#include "shell.h"
#include "keyboard.h"

/*
void shell_handle_echo() {
    for (int i = 1; i < g_NumWords; i++) {
        if (i == g_NumWords - 1)  printf("%s", g_CurrentCommand[i]);
        else printf("%s ", g_CurrentCommand[i]);
    }
}
*/

void shell_handle_kbtest() {
    printf("disabling keyboard ...\t");
    kb_disable();
    printf("%s", kb_state() ? "ENABLED\n" : "DISABLED\n");
    printf("enabling keyboard ... \t");
    kb_enable();
    printf("%s", kb_state() ? "ENABLED" : "DISABLED");
}

/*
void shell_add_command_word(const char* word_str, uint8_t len_word) {
    if (g_NumWords < 16) {
        uint8_t copy_len = (len_word < 255) ? len_word : 255;
        memcpy(g_CommandArgsBuffer[g_NumWords], word_str, copy_len);

        g_CommandArgsBuffer[g_NumWords][copy_len] = '\0';

        g_CurrentCommand[g_NumWords] = g_CommandArgsBuffer[g_NumWords];
        g_NumWords++;
    }
}
*/

void get_tokens(shell_ctx* ctx, char* str) {
    ctx->words = 0;
}

void shell_handle_input() {
    char* buffer = kb_get_buffer();

    // init context
    shell_ctx ctx;
    ctx.capacity = 8;
    ctx.count = 0;

    get_tokens(&ctx, buffer);


    kb_clear_buffer();
    printf("\n> ");
}

