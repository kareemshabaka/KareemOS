.set ALIGN,    1<<0
.set MEMINFO,  1<<1
.set FLAGS,    ALIGN | MEMINFO
.set MAGIC,    0x1BADB002
.set CHECKSUM, -(MAGIC + FLAGS)

.section .multiboot
.align 4
    .long MAGIC
    .long FLAGS
    .long CHECKSUM

.section .bss
.align 16
stack_bottom:
    .skip 16384 # 16KB
stack_top:

.section .text
.global _start
.type _start, @function
_start:
    mov $stack_top, %esp
    push %ebx
    call kernel_main
    cli
1:  hlt
    jmp 1b

.global init_gdt
.type init_gdt, @function
init_gdt:
    mov 4(%esp), %eax
    lgdt (%eax)

    mov $0x10, %ax      
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %ss

    ljmp $0x08, $.flush
.flush:
    ret

.global init_idt
.type init_idt, @function
init_idt:
    mov 4(%esp), %eax
    lidt (%eax)
    ret
.size _start, . - _start