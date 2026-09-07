#include <stdint.h>
#include "io.h"
#include "irq.h"
#include "idt.h"
#include "isr.h"
#include "../../drivers/vga.h"

void pic_send_eoi(uint8_t irq)
{
    if (irq >= 8)
        outb(0xA0, 0x20);

    outb(0x20, 0x20); // this isnt part of the if... do. not.
}

static void (*irq_handlers[16])(struct registers *r);

void irq_handler(struct registers *r)
{
    uint8_t irq = r->int_no - 32;

    if (irq_handlers[irq]) {
        irq_handlers[irq](r);
    }

    pic_send_eoi(irq);
}

void irq_install_handler(uint8_t irq, void (*handler)(struct registers *r))
{
    irq_handlers[irq] = handler;
}

void irq_install(void)
{
    // ICW1
    outb(0x20, 0x11); io_wait();
    outb(0xA0, 0x11); io_wait();
    // ICW2
    outb(0x21, 32);   io_wait();
    outb(0xA1, 40);   io_wait();
    // ICW3
    outb(0x21, 0x04); io_wait();
    outb(0xA1, 0x02); io_wait();
    // ICW4
    outb(0x21, 0x01); io_wait();
    outb(0xA1, 0x01); io_wait();
    // Unmask IRQs
    outb(0x21, 0x00);
    outb(0xA1, 0x00);

    // Wire IDT Gates 32-47 to IRQ Stubs
    idt_set_gate(32, (uint32_t)irq0,  0x08, 0x8E);
    idt_set_gate(33, (uint32_t)irq1,  0x08, 0x8E);
    idt_set_gate(34, (uint32_t)irq2,  0x08, 0x8E);
    idt_set_gate(35, (uint32_t)irq3,  0x08, 0x8E);
    idt_set_gate(36, (uint32_t)irq4,  0x08, 0x8E);
    idt_set_gate(37, (uint32_t)irq5,  0x08, 0x8E);
    idt_set_gate(38, (uint32_t)irq6,  0x08, 0x8E);
    idt_set_gate(39, (uint32_t)irq7,  0x08, 0x8E);
    idt_set_gate(40, (uint32_t)irq8,  0x08, 0x8E);
    idt_set_gate(41, (uint32_t)irq9,  0x08, 0x8E);
    idt_set_gate(42, (uint32_t)irq10,  0x08, 0x8E);
    idt_set_gate(43, (uint32_t)irq11,  0x08, 0x8E);
    idt_set_gate(44, (uint32_t)irq12,  0x08, 0x8E);
    idt_set_gate(45, (uint32_t)irq13,  0x08, 0x8E);
    idt_set_gate(46, (uint32_t)irq14,  0x08, 0x8E);
    idt_set_gate(47, (uint32_t)irq15,  0x08, 0x8E);
}