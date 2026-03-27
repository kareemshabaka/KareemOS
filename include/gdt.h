#ifndef GDT_T
#define GDT_T

#include "types.h"

#define GDT_ENTRIES 3

// ACCESS
#define SEG_PRES(x) ((x) << 0x07) // Present bit
#define SEG_DPL(x) ((x) << 0x05)  // Descriptor privilege level field
#define SEG_DESC(x) ((x) << 0x04) // Descriptor type bit
#define SEG_EXEC(x) ((x) << 0x03) // Executable bit
#define SEG_DIR(x) ((x) << 0x02)  // Direction bit/Conforming bit
#define SEG_RW(x) ((x) << 0x01)   // Readable bit/Writable bit
#define SEG_ACCESS(x) ((x))       // Accessed bit

// FLAGS
#define FLAG_GRAN(x)                                                                               \
    ((x) << 0x03) // Granularity flag, indicates the size the Limit value is scaled by
#define FLAG_SIZE(x) ((x) << 0x02) // Size flag
#define FLAG_LONG(x) ((x) << 0x01) // Long-mode code flag

struct gdt_descriptor_args
{
    int num;
    uint32_t base;
    uint32_t limit;
    uint8_t access;
    uint8_t flags;
};

struct gdt_entry
{
    uint16_t limit_low;  // 0:15
    uint16_t base_low;   // 16:31
    uint8_t base_middle; // 32:39
    uint8_t access;      // 40:47
    uint8_t flags;       // 48:55 - 48:51 is Limit, 52:55 is Flags
    uint8_t base_high;   // 56:63
} __attribute__((packed));

struct gdt_ptr
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

extern struct gdt_entry gdt[GDT_ENTRIES];
extern struct gdt_ptr gdtr;

void create_gdt_descriptor(struct gdt_descriptor_args args);
void create_gdt();
void init_gdt(struct gdt_ptr *gdtr);

#endif