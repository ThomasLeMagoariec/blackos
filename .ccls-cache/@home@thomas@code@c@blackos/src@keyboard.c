#include "keyboard.h"

uint16_t g_KeyboardBuffer[MAX_KB_SIZE];
volatile uint8_t kbd_buffer[MAX_KB_SIZE];
volatile int kb_head = 0;
volatile int kb_tail = 0;

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
