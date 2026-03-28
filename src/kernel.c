#include "gdt.h"
#include "idt.h"
#include "kernel.h"
#include "keyboard.h"
#include "mem.h"
#include "paging.h"
#include "pic.h"
#include "shell.h"
#include "vga.h"

#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

#if !defined(__i386__)
#error "Need to be compiled with a ix86-elf compiler"
#endif

void kernel_main(void)
{
    create_gdt();
    idt_install();
    PIC_remap(0x20, 0x28);
    asm volatile("sti");
    clear_screen();

    init_shell();

    while (1) {
        unsigned char c = kbuffer_dequeue();

        if (c == 0) {
            halt();
            continue;
        }

        handle_input(c);
    }
}