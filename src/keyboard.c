#include "keyboard.h"
#include "shell.h"

#define keymap          keymap_azerty
#define keymap_shift    keymap_azerty_shift

char g_KeyboardBuffer[MAX_KB_SIZE];
uint8_t kb_buf;

volatile uint8_t kbd_buffer[MAX_KB_SIZE];
volatile int kb_head = 0;
volatile int kb_tail = 0;

int shift_pressed = 0;

char keymap_azerty[128] = {
    0, 27, '&','é','"','\'','(','-','_','_','ç','à',')','=','\b',
    '\t', 'a','z','e','r','t','y','u','i','o','p','^','$','\n',
    0, 'q','s','d','f','g','h','j','k','l','m','ù','`', 0,
    '\\','w','x','c','v','b','n',',',';',':','!', 0, '*', 0, ' '
};

static const char keymap_azerty_shift[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','°','+','\b',
    '\t','A','Z','E','R','T','Y','U','I','O','P','¨','£','\n',
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

char map_scancode(uint8_t scancode) {
    char c = shift_pressed ? keymap_shift[scancode] : keymap[scancode];
    if(c >= 'a' && c <= 'z' && shift_pressed) c -= 32; // uppercase

    if (kb_buf < MAX_KB_SIZE)
      g_KeyboardBuffer[kb_buf++] = c;

    return c;
}

void clear_kbbuffer() {
    for (uint8_t i = 0; i < 128; i++) {
        g_KeyboardBuffer[i] = 0;
    };
    kb_buf = 0;
}

char* get_kbbuffer() {
    return g_KeyboardBuffer;
}

void handle_scancode(uint8_t scancode) {
    int released = scancode & 0x80;
    uint8_t make = scancode & 0x7F;

    switch(make) {
        case 0x2A: // left shift
        case 0x36: // right shift
            shift_pressed = !released;
            return;
        case 0x1C: // enter
            if (!released) {
                printf("\n");
                handle_input();
            }

            return;
        case 0x0E: // backspace
            if (!released) {
                backspace();
                kb_buf--;
                g_KeyboardBuffer[kb_buf] = '\0';
            }
            return;
    }

    if(!released) enqueue_key(make); // only store key presses
}


void enqueue_key(uint8_t scancode) {
    int next = (kb_head + 1) % MAX_KB_SIZE;
    if(next != kb_tail) { // buffer not full
        kbd_buffer[kb_head] = scancode;
        kb_head = next;
    }
}

int dequeue_key() {
    if(kb_tail == kb_head) return -1; // buffer empty
    int scancode = kbd_buffer[kb_tail];
    kb_tail = (kb_tail + 1) % MAX_KB_SIZE;
    return scancode;
}
