ENTRY(_start)

SECTIONS
{
    . = 1M;

    .text : {
        KEEP(*(.multiboot))
        *(.text)
        *(.rodata)
    } :text

    .data : ALIGN(4K)
    {
        *(.data)
    } :text

    .bss : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
    } :text
}
