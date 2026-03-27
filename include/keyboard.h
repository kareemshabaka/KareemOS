#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "types.h"

#define KBD_BUFFER_SIZE 256

#define KEY_UP 0xE1
#define KEY_DOWN 0xE2
#define KEY_LEFT 0xE3
#define KEY_RIGHT 0xE4

typedef struct {
    unsigned char data[KBD_BUFFER_SIZE];
    int head;
    int tail;
} key_buffer_t;

extern key_buffer_t kbd_buffer;

void kbuffer_enqueue(unsigned char c);
unsigned char kbuffer_dequeue();

void scancode_handler(uint8_t scancode);

#endif