#include "idt.h"
#include "io.h"
#include "keyboard.h"
#include "vga.h"

#define INT_GATE (ATTR_PRES(1) | ATTR_DPL(0) | ATTR_TYPE(GATE_INT))
#define TRAP_GATE (ATTR_PRES(1) | ATTR_DPL(0) | ATTR_TYPE(GATE_TRAP))
#define TASK_GATE (ATTR_PRES(1) | ATTR_DPL(0) | ATTR_TYPE(GATE_TASK))

struct idt_entry idt[IDT_ENTRIES];
struct idt_ptr idtr;

void *isr_stub_table[] = {isr0,  isr1,  isr2,  isr3,  isr4,  isr5,  isr6,  isr7,
                          isr8,  isr9,  isr10, isr11, isr12, isr13, isr14, isr15,
                          isr16, isr17, isr18, isr19, isr20, isr21, isr22, isr23,
                          isr24, isr25, isr26, isr27, isr28, isr29, isr30, isr31};

void *irq_stub_table[] = {irq0, irq1, irq2,  irq3,  irq4,  irq5,  irq6,  irq7,
                          irq8, irq9, irq10, irq11, irq12, irq13, irq14, irq15};

void idt_set_gate(struct idt_descriptor_args args)
{
    struct idt_entry entry;

    entry.offset_low = args.offset & 0xFFFF;
    entry.offset_high = (args.offset >> 16) & 0xFFFF;

    entry.sel = args.sel;

    entry.attr = args.attr;
    entry.zero = 0;

    idt[args.num] = entry;
}
void idt_install()
{
    idtr.limit = (sizeof(struct idt_entry) * IDT_ENTRIES) - 1;
    idtr.base = (uint32_t)&idt;

    for (int i = 0; i < 32; ++i) {
        idt_set_gate((struct idt_descriptor_args){.num = i,
                                                  .offset = (uint32_t)isr_stub_table[i],
                                                  .sel = 0x08, // Kernel Code Segment
                                                  .attr = INT_GATE});
    }

    for (int i = 0; i < 16; ++i) {
        idt_set_gate((struct idt_descriptor_args){.num = i + 32,
                                                  .offset = (uint32_t)irq_stub_table[i],
                                                  .sel = 0x08, // Kernel Code Segment
                                                  .attr = INT_GATE});
    }

    init_idt(&idtr);
}

void exception_handler(struct registers r)
{
    clear_screen();
    kputs("ISR HIT\n");
    kprintn(r.int_no, 10);
    kputs("\nEIP: ");
    kprintn(r.eip, 16);
    for (;;) {
        ;
    }
}

void keyboard_handler()
{
    uint8_t scancode = inb(0x60);
    scancode_handler(scancode);
}

void isr_handler(struct registers r)
{
    if (r.int_no >= 32 && r.int_no <= 47) {
        if (r.int_no >= 40) {
            outb(0xA0, 0x20); // Send EOI to Slave
        }
        outb(0x20, 0x20); // Send EOI to Master
    }
    switch (r.int_no) {
    case 0 ... 31:
        exception_handler(r);
        break;
    case 32: // Timer (IRQ 0)
        // kputs("TICK");
        //  tick++;
        break;

    case 33:
        keyboard_handler();
        break;

    default:
        kputs("INVALID ISR");
        break;
    }
}