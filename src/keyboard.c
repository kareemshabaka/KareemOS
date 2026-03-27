#include "keyboard.h"

unsigned char kbd_us[128] = {0,   27,  '1',  '2',  '3',  '4', '5', '6',  '7', '8', '9', '0',
                             '-', '=', '\b', '\t', 'q',  'w', 'e', 'r',  't', 'y', 'u', 'i',
                             'o', 'p', '[',  ']',  '\n', 0,   'a', 's',  'd', 'f', 'g', 'h',
                             'j', 'k', 'l',  ';',  '\'', '`', 0,   '\\', 'z', 'x', 'c', 'v',
                             'b', 'n', 'm',  ',',  '.',  '/', 0,   '*',  0,   ' '};

unsigned char kbd_caps_us[128] = {0,   27,  '!',  '@',  '#',  '$', '%', '^',  '&', '*', '(', ')',
                                  '_', '+', '\b', '\t', 'Q',  'W', 'E', 'R',  'T', 'Y', 'U', 'I',
                                  'O', 'P', '{',  '}',  '\n', 0,   'A', 'S',  'D', 'F', 'G', 'H',
                                  'J', 'K', 'L',  ':',  '"',  '`', 0,   '\\', 'Z', 'X', 'C', 'V',
                                  'B', 'N', 'M',  '<',  '>',  '?', 0,   '*',  0,   ' '};

static int shift_pressed = 0;
static int is_extended = 0;

key_buffer_t kbd_buffer;

void scancode_handler(uint8_t scancode)
{
    if (scancode == 0xE0) {
        is_extended = 1;
        return;
    }

    if (is_extended) {
        is_extended = 0;
        switch (scancode) {
        case 0x48:
            kbuffer_enqueue(KEY_UP);
            return;
        case 0x50:
            kbuffer_enqueue(KEY_DOWN);
            return;
        case 0x4B:
            kbuffer_enqueue(KEY_LEFT);
            return;
        case 0x4D:
            kbuffer_enqueue(KEY_RIGHT);
            return;
        default:
            return;
        }
    }

    if (scancode == 0x2A || scancode == 0x36) {
        shift_pressed = 1;
        return;
    }

    if (scancode == 0xAA || scancode == 0xB6) {
        shift_pressed = 0; // Shift Released
        return;
    }

    if (scancode & 0x80) {
        return;
    }

    if (scancode >= 128) {
        return;
    }

    unsigned char c = shift_pressed ? kbd_caps_us[scancode] : kbd_us[scancode];
    if (c > 0) {
        kbuffer_enqueue(c);
    }
}

void kbuffer_enqueue(unsigned char c)
{
    kbd_buffer.data[kbd_buffer.head] = c;
    int new_head = kbd_buffer.head + 1;
    if (new_head >= KBD_BUFFER_SIZE) {
        new_head = 0;
    }
    kbd_buffer.head = new_head;
}

unsigned char kbuffer_dequeue()
{
    if (kbd_buffer.tail == kbd_buffer.head) {
        return 0;
    }
    unsigned char c = kbd_buffer.data[kbd_buffer.tail];

    int new_tail = kbd_buffer.tail + 1;
    if (new_tail >= KBD_BUFFER_SIZE) {
        new_tail = 0;
    }
    kbd_buffer.tail = new_tail;
    return c;
}