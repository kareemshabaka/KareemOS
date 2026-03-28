#ifndef SHELL_H
#define SHELL_H

#define BUFFER_SIZE 256
#define MAX_HISTORY 16
#define HISTORY_MASK (MAX_HISTORY - 1)
#define MAX_ARGS 16

#include "types.h"

extern char buf[BUFFER_SIZE];
extern size_t history[MAX_HISTORY];

typedef void (*command_func_t)(char *args[]);

typedef struct
{
    const char *name;
    const char *description;
    command_func_t function;
} shell_command_t;

void cmd_help(char *args[]);
void cmd_clear(char *args[]);
void cmd_reboot(char *args[]);
void cmd_history(char *args[]);
void cmd_echo(char *args[]);

void handle_input(unsigned char c);
void init_shell();
void execute_command();
void store_history(size_t cmd_idx);

#endif