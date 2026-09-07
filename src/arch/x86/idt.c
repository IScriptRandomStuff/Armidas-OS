#include "idt.h"

#define IDT_ENTRIES          256
#define KERNEL_CS            0x08
#define IDT_FLAG_KERNEL_GATE 0x8E

struct idt_entry {
    uint16_t base_low;
    uint16_t selector;
    uint8_t  reserved;
    uint8_t  flags;
    uint16_t base_high;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

static struct idt_entry idt[IDT_ENTRIES];
static struct idt_ptr   idtp;

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
{
    idt[num].base_low  = (uint16_t)(base & 0xFFFF);
    idt[num].base_high = (uint16_t)(base >> 16);
    idt[num].selector  = sel;
    idt[num].reserved  = 0;
    idt[num].flags     = flags;
}

__attribute__((naked))
static void unhandled_isr(void)
{
    __asm__ __volatile__("iret");
}

void idt_install(void)
{
    idtp.limit = (uint16_t)(sizeof(idt) - 1);
    idtp.base  = (uint32_t)idt;

    for (int i = 0; i < IDT_ENTRIES; i++)
        idt_set_gate((uint8_t)i, (uint32_t)unhandled_isr, KERNEL_CS, IDT_FLAG_KERNEL_GATE);

    __asm__ __volatile__("lidt %0" : : "m"(idtp) : "memory");
}