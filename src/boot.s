.set ALIGN,    1<<0
.set MEMINFO,  1<<1
.set FLAGS,    ALIGN | MEMINFO
.set MAGIC,    0x1BADB002
.set CHECKSUM, -(MAGIC + FLAGS)

.section .multiboot, "aw"
.align 4
    .long MAGIC
    .long FLAGS
    .long CHECKSUM

.section .bss, "aw", @nobits
.global boot_page_directory
.align 4096
boot_page_directory:
    .skip 4096
boot_page_table1:
    .skip 4096

.align 16
stack_bottom:
    .skip 16384 # 16KB
stack_top:

.intel_syntax noprefix
.code32

.section .boot
.global _start
_start:
    mov edi, OFFSET boot_page_table1 - 0xC0000000

    mov esi, 0
    mov ecx, 1024

.fill_pt:
    mov edx, esi
    or edx, 0x3
    mov [edi], edx

    add esi, 4096
    add edi, 4
    loop .fill_pt

.done_fill:
    # mov dword ptr [boot_page_table1 - 0xC0000000 + 1023 * 4], 0x000B8003
    mov eax, OFFSET boot_page_table1 - 0xC0000000
    or eax, 0x003

    mov dword ptr [boot_page_directory - 0xC0000000 + 0], eax
    mov dword ptr [boot_page_directory - 0xC0000000 + 768 * 4], eax

    mov ecx, OFFSET boot_page_directory - 0xC0000000

    mov cr3, ecx

    mov ecx, cr0
    or ecx, 0x80010000
    mov cr0, ecx

    lea ecx, [_main]
    jmp ecx


.size _start, . - _start

.section .text
_main:
    mov esp, OFFSET stack_top

    mov eax, OFFSET boot_page_directory
    mov dword ptr [eax], 0
    invlpg [0]

    call kernel_main
    cli
1:  hlt
    jmp 1b

.global init_gdt
init_gdt:
    mov eax, [esp + 4]
    lgdt [eax]

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    push 0x08
    lea eax, [.flush]
    push eax
    retf
.flush:
    ret

.global init_idt
init_idt:
    mov eax, [esp + 4]
    lidt [eax]
    ret