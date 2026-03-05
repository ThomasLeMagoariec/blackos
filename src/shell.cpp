#include "shell.hpp"
#include "keyboard.h"
#include "string.h"


void kbtest() {
    printf("disabling keyboard ...\t");
    kb_disable();
    printf("%s", kb_state() ? "ENABLED\n" : "DISABLED\n");
    printf("enabling keyboard ... \t");
    kb_enable();
    printf("%s", kb_state() ? "ENABLED" : "DISABLED");
}


void echo(shell_ctx* ctx) {
    for (int i = 1; i < ctx->count; i++) {
        if (i <= ctx->count - 2) printf("%s ", ctx->words[i]);
        else                    printf(ctx->words[i]);
    }

}

void exec(shell_ctx* ctx) {
   if (ctx->count == 0) return; 

   if (strcmp(ctx->words[0], "echo") == 0) {
       echo(ctx);
   } else if (strcmp(ctx->words[0], "kbtest") == 0) {
       kbtest();
   } else {
        printf("UNKNOWN\n");
   }
}

void get_tokens(shell_ctx* ctx, char* str) {
    ctx->words = (char**)malloc(ctx->capacity * sizeof(char *));

    char* word;

    word = strtok(str, " ");
    while (word != NULL) {
        if (ctx->count >= ctx->capacity) {
            ctx->capacity *= 2;
            ctx->words = (char **)realloc(ctx->words, ctx->capacity * sizeof(char *));

            if (!ctx->words) {
                printf("realloc error!!!!");
            }
        }

        ctx->words[ctx->count] = strdup(word);
        ctx->count++;
        word = strtok(NULL, " ");
    }

    ctx->words[ctx->count] = NULL;
}

void shell_handle_input() {
    char* buffer = kb_get_buffer();

    // init context
    shell_ctx ctx;
    ctx.capacity = 8;
    ctx.count = 0;

    get_tokens(&ctx, buffer);
    exec(&ctx);


    kb_clear_buffer();
    printf("\n> ");
}

