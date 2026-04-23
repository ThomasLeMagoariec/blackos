#include "keyboard.hpp"
#include "dbg_stdio.h"

#define keymap          keymap_azerty
#define keymap_shift    keymap_azerty_shift


volatile int kb_head = 0;
volatile int kb_tail = 0;

//int shift_pressed = 0;
//uint8_t kb_enabled = 1;

bool mapped = false;

kb_event g_kb_events[256];
int g_kbEventNum;

kb_ctx g_ctx;

char keymap_azerty[128] = {
    0, 27, '&', 0,'"','\'','(','-','_','_', 0, 0,')','=','\b',
    '\t', 'a','z','e','r','t','y','u','i','o','p','^','$','\n',
    0, 'q','s','d','f','g','h','j','k','l','m', 0,'`', 0,
    '\\','w','x','c','v','b','n',',',';',':','!', 0, '*', 0, ' '
};

static const char keymap_azerty_shift[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0', 0,'+','\b',
    '\t','A','Z','E','R','T','Y','U','I','O','P', 0, 0,'\n',
    0, 'Q','S','D','F','G','H','J','K','L','M','%','~',
    0, '|','W','X','C','V','B','N','M','?','.','/',
    0, '*', 0, ' '
};

char keymap_qwerty[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=','\b',
    '\t', 'q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0, 'a','s','d','f','g','h','j','k','l',';','\'','`', 0,
    '\\','z','x','c','v','b','n','m',',','.','/', 0, '*', 0, ' '
};

static const char keymap_qwerty_shift[128] = {
    0, 27, '!','@','#','$','%','^','&','*','(',')','_','+','\b',
    '\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n',
    0, 'A','S','D','F','G','H','J','K','L',':','"','~',
    0, '|','Z','X','C','V','B','N','M','<','>','?',
    0, '*', 0, ' '
};


void kb_init() {
    g_ctx.shift_pressed = false;
    g_ctx.enabled = true;

    // event handlers
    g_ctx.numHandlers = 0;
    //g_ctx.handlers = (kb_event*)malloc(sizeof(kb_event));
    g_ctx.handlers = NULL;
}

char kb_map_scancode(uint8_t scancode) {
    mapped = true;

    char c = g_ctx.shift_pressed ? keymap_shift[scancode] : keymap[scancode];
    if (c >= 'a' && c <= 'z' && g_ctx.shift_pressed) c -= 32; // uppercase

    if (g_ctx.bufferPos < MAX_KB_SIZE && c != 0)
      g_ctx.buffer[g_ctx.bufferPos++] = c;

    if (scancode == 0x0E) {
        dbg_printf("backspace scancode %d\n", c);
    }

    return c;
}

void kb_clear_buffer() {
    for (uint8_t i = 0; i < 128; i++) {
        g_ctx.buffer[i] = 0;
    };
    g_ctx.bufferPos = 0;
}

char* kb_get_buffer() {
    return g_ctx.buffer;
}

uint8_t kb_get_size() {
    return g_ctx.bufferPos;
}

void kb_disable() {
    g_ctx.enabled = 0;
}

void kb_enable() {
    g_ctx.enabled = 1;
}

void kb_toggle() {
    g_ctx.enabled = !g_ctx.enabled;
}

uint8_t kb_state() {
    return g_ctx.enabled;
}

void register_kbevent(kb_event event) {
    int num = g_ctx.numHandlers == 0 ? 1 : g_ctx.numHandlers;

    g_ctx.handlers = (kb_event*)realloc(g_ctx.handlers, sizeof(kb_event) * num);
    g_ctx.handlers[g_ctx.numHandlers] = event;
    g_ctx.numHandlers++;
}

void kb_main_event(uint8_t scancode) {


    mapped = false;

    int released = scancode & 0x80;
    uint8_t make = scancode & 0x7F;

    switch(make) {
        case 0x2A: // left shift
        case 0x36: // right shift
            g_ctx.shift_pressed = !released;
            return;
        case 0x0E: // backspace
            if (!released) {
                dbg_printf("trigger backsapc\n");
                backspace();


                // no clue why this works
                g_ctx.bufferPos--;
                g_ctx.buffer[g_ctx.bufferPos--] = '\0';
            }
            return;

    }
}

void kb_handle_scancode(uint8_t scancode) {

    for (int i = 0; i < g_ctx.numHandlers; i++) {
        g_ctx.handlers[i](scancode);
    }
}

