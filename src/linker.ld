ENTRY(_start)

SECTIONS
{
    . = 0x00100000;
    _kernel_start = .;

    .boot : ALIGN(4K) {
        KEEP(*(.multiboot))
        *(.boot)
    }

    . += 0xC0000000;

    .text : AT (ADDR(.text) - 0xC0000000) {
        *(.text)
        *(.rodata)
    }

    .data : AT(ADDR(.data) - 0xC0000000) ALIGN(4K)
    {
        *(.data)
    }

    .bss : AT(ADDR(.bss) - 0xC0000000) ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
    }
    _kernel_end = .;
}
