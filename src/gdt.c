#include "gdt.h"
#include "types.h"

#define KERN_CODE_ACCESS (SEG_PRES(1) | SEG_DESC(1) | SEG_EXEC(1) | SEG_RW(1))
#define KERN_DATA_ACCESS (SEG_PRES(1) | SEG_DESC(1) | SEG_EXEC(0) | SEG_RW(1))
#define KERN_FLAGS (FLAG_GRAN(1) | FLAG_SIZE(1))

struct gdt_entry gdt[GDT_ENTRIES];
struct gdt_ptr gdtr;

// NOLINTBEGIN(readability-magic-numbers)
void create_gdt_descriptor(struct gdt_descriptor_args args)
{
    struct gdt_entry entry;

    entry.base_high = (args.base >> 24) & 0xFF;
    entry.base_middle = (args.base >> 16) & 0xFF;
    entry.base_low = (args.base & 0xFFFF);

    entry.limit_low = (args.limit & 0xFFFF);

    entry.flags = (args.limit >> 16) & 0x0F;
    entry.flags |= (args.flags << 4 & 0xF0);

    entry.access = args.access;

    gdt[args.num] = entry;
}

void create_gdt()
{
    create_gdt_descriptor((struct gdt_descriptor_args){0, 0, 0, 0, 0});
    create_gdt_descriptor(
        (struct gdt_descriptor_args){1, 0x0, 0xFFFFF, KERN_CODE_ACCESS, KERN_FLAGS});
    create_gdt_descriptor(
        (struct gdt_descriptor_args){2, 0x0, 0xFFFFF, KERN_DATA_ACCESS, KERN_FLAGS});

    gdtr.base = (uint32_t)&gdt;
    gdtr.limit = (sizeof(struct gdt_entry) * GDT_ENTRIES) - 1;

    init_gdt(&gdtr);

}
// NOLINTEND(readability-magic-numbers)