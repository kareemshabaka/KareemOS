#include "vga.h"
#include "io.h"
#include "klib.h"
#include "mem.h"
#include "string.h"

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t *terminal_buffer = (uint16_t *)VGA_MEMORY;

static inline uint16_t vga_entry(unsigned char uc, uint8_t color)
{
    return (uint16_t)uc | (uint16_t)color << 8;
}

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg)
{
    return fg | bg << 4;
}

void update_cursor()
{
    size_t index = (terminal_row * VGA_WIDTH) + terminal_column;

    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((index >> 8) & 0xFF));

    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(index & 0xFF));
}

void shift_cursor(int dir)
{
    if (dir == 1) {
        if (++terminal_column == VGA_WIDTH) {
            terminal_column = 0;
            if (++terminal_row == VGA_HEIGHT) {
                terminal_row = VGA_HEIGHT - 1;
            }
        }
    }
    else {
        if (terminal_column > 0) {
            terminal_column--;
        }
        else if (terminal_row > 0) {
            terminal_row--;
            terminal_column = VGA_WIDTH - 1;
        }
    }

    update_cursor();
}

void shift_text(int dir, size_t count)
{
    size_t index = (terminal_row * VGA_WIDTH) + terminal_column;
    if (dir == 1) {
        for (size_t i = index + count; i > index; --i) {
            terminal_buffer[i] = terminal_buffer[i - 1];
        }
    }
    else {
        for (size_t i = index; i < index + count; ++i) {
            terminal_buffer[i] = terminal_buffer[i + 1];
        }
        terminal_buffer[index + count] = vga_entry(' ', terminal_color);
    }
}

void clear_screen()
{
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

    memsetw(terminal_buffer, vga_entry(' ', terminal_color), VGA_WIDTH * VGA_HEIGHT);
    update_cursor();
}

void scroll_screen()
{
    size_t start_index = VGA_WIDTH;

    for (size_t i = 0; i < (VGA_WIDTH * VGA_HEIGHT) - VGA_WIDTH; ++i) {
        terminal_buffer[i] = terminal_buffer[start_index];
        ++start_index;
    }

    start_index = VGA_WIDTH * (VGA_HEIGHT - 1);
    for (size_t i = 0; i < VGA_WIDTH; ++i) {
        terminal_buffer[start_index + i] = vga_entry(' ', terminal_color);
    }
}

void insert_char(const char c)
{
    switch (c) {
    case '\n':
        terminal_column = 0;

        if (++terminal_row == VGA_HEIGHT) {
            terminal_row = VGA_HEIGHT - 1;
            scroll_screen();
        }
        return;

    case '\b':

        if (terminal_column > 0) {
            terminal_column--;
        }
        else if (terminal_row > 0) {
            terminal_row--;
            terminal_column = VGA_WIDTH - 1;
        }
        else {
            return;
        }
        size_t index = (terminal_row * VGA_WIDTH) + terminal_column;
        terminal_buffer[index] = vga_entry(' ', terminal_color);
        return;

    default:
        break;
    }

    size_t index = (terminal_row * VGA_WIDTH) + terminal_column;
    terminal_buffer[index] = vga_entry(c, terminal_color);

    if (++terminal_column == VGA_WIDTH) {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) {
            scroll_screen();
        }
    }
}

void kputs(const char *str)
{
    size_t size = strlen(str);
    for (size_t i = 0; i < size; ++i) {
        insert_char(str[i]);
    }
    update_cursor();
}

void kputc(char c)
{
    insert_char(c);
    update_cursor();
}

void kprintn(uint32_t n, int base)
{
    char buf[12];
    if (base == 16) {
        itohex(n, buf);
        kputs(buf);
    }
    else {
        itoa(n, buf);
        kputs(buf);
    }
    update_cursor();
}
