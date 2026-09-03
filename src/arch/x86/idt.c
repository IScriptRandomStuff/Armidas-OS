#include "idt.h"

// One IDT entry describes one interrupt handler
struct idt_entry {
    uint16_t base_low;   // lower 16 bits of handler function address
    uint16_t selector;   // GDT code segment selector (0x08)
    uint8_t  always0;    // always zero
    uint8_t  flags;      // type and privilege flags
    uint16_t base_high;  // upper 16 bits of handler function address
} __attribute__((packed));

// This gets loaded into the CPU's IDTR register
struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

// 256 possible interrupts (CPU exceptions + hardware IRQs + software)
static struct idt_entry idt[256];
static struct idt_ptr   idtp;

// Set one entry in the IDT
static void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
{
    idt[num].base_low  = (base & 0xFFFF);
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector  = sel;
    idt[num].always0   = 0;
    idt[num].flags     = flags;
}

// Default handler — catches any interrupt we haven't handled yet
static void default_handler(void)
{
    // just return for now, we'll add real handlers later
}

void idt_install(void)
{
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base  = (uint32_t)&idt;

    // Zero out the entire IDT first
    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, (uint32_t)default_handler, 0x08, 0x8E);
    }

    // Load the IDT into the CPU
    __asm__ __volatile__(
        "lidt %0"
        :
        : "m"(idtp)
    );
}