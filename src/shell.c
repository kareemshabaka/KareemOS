#include "shell.h"
#include "io.h"
#include "kernel.h"
#include "keyboard.h"
#include "klib.h"
#include "vga.h"
#include <stddef.h>
#include <string.h>

char buf[BUFFER_SIZE];
size_t history[MAX_HISTORY];

size_t history_head = 0;
size_t history_tail = 0;

size_t pos = 0;
size_t count = 0;

shell_command_t command_table[] = {
    {"help", "List all commands", cmd_help},
    {"clear", "Clear the screen", cmd_clear},
    {"reboot", "Restart the system", cmd_reboot},
    {"history", "Command history", cmd_history},
    {0, 0, 0} // Null terminator to find the end of the array
};

void execute_command()
{
    for (int i = 0; command_table[i].name != 0; i++) {
        if (strcmp(buf, command_table[i].name) == 0) {
            store_history(i);
            command_table[i].function(NULL);
            return;
        }
    }

    kputs("Unknown command. Type 'help' for a list.\n");
}

void store_history(size_t cmd_idx)
{
    history[history_head] = cmd_idx;

    history_head = (history_head + 1) & HISTORY_MASK;

    if (history_head == history_tail) {
        history_tail = (history_tail + 1) & HISTORY_MASK;
    }
}

void init_shell()
{
    kputs("shell> ");
}

void cmd_help([[maybe_unused]] char *args)
{
    kputs("Available commands:\n");
    for (int i = 0; command_table[i].name != 0; i++) {
        kputs("  ");
        kputs(command_table[i].name);
        kputs(" - ");
        kputs(command_table[i].description);
        kputs("\n");
    }
}

void cmd_clear([[maybe_unused]] char *args)
{
    clear_screen();
}

void cmd_reboot([[maybe_unused]] char *args)
{
    uint8_t good = 0x02;
    while (good & 0x02) {
        good = inb(0x64);
    }
    outb(0x64, 0xFE);
}

void cmd_history([[maybe_unused]] char *args)
{
    size_t tmp = history_tail;
    size_t count = 1;
    while (tmp != history_head) {
        size_t idx = history[tmp];
        kprintn(count, 10);
        kputs(". ");
        kputs(command_table[idx].name);
        kputs("\n");
        tmp = (tmp + 1) & HISTORY_MASK;
        count++;
    }
}

void shift_buffer(int dir)
{
    if (dir == 1) {
        for (size_t i = (BUFFER_SIZE - 1); i > pos; --i) {
            buf[i] = buf[i - 1];
        }
    }
    else {
        for (size_t i = pos; i < (BUFFER_SIZE - 1) - 1; ++i) {
            buf[i] = buf[i + 1];
        }
    }
}

void handle_input(unsigned char c)
{
    if (c == '\n') {

        kputc('\n');

        if (count > 0) {
            buf[count] = '\0';
            execute_command();
        }

        count = 0;
        pos = 0;
        kputs("shell> ");
    }
    else if (c == '\b') {
        if (pos > 0) {
            int is_inserting = pos < count;

            pos--;
            count--;

            kputc('\b');

            if (is_inserting) {
                shift_buffer(-1);
                shift_text(-1, count);
            }
            else {
                kputc(' ');
                kputc('\b');
            }
        }
    }
    else if (c == KEY_RIGHT) {
        if (pos < count) {
            shift_cursor(1);
            pos++;
        }
    }
    else if (c == KEY_LEFT) {
        if (pos > 0) {
            shift_cursor(-1);
            pos--;
        }
    }
    else if (count < BUFFER_SIZE) {
        int is_inserting = pos < count;
        if (is_inserting) {
            shift_buffer(1);
            shift_text(1, count);
        }
        buf[pos] = (char)c;
        pos++;
        count++;

        kputc((char)c);
    }
}