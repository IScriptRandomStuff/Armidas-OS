#include "idt.h"
#include <stddef.h>

/* -----------------------------------------------------------------------
 * Types & constants
 * ----------------------------------------------------------------------- */

#define IDT_ENTRIES   256
#define KERNEL_CS     0x08

/* Gate type + DPL + present bit packed into the flags byte:
 *   Bit 7    : Present (P)
 *   Bits 6-5 : Descriptor Privilege Level (DPL)
 *   Bit 4    : Storage segment (always 0 for interrupt gates)
 *   Bits 3-0 : Gate type
 *
 *   0x8E = 1_00_0_1110b  →  Present, DPL=0, 32-bit interrupt gate
 *   0xEE = 1_11_0_1110b  →  Present, DPL=3, 32-bit interrupt gate (user-callable)
 */
#define IDT_FLAG_KERNEL_GATE  0x8E
#define IDT_FLAG_USER_GATE    0xEE

/* A handler function pointer */
idt_set_gate(32, (uint32_t)irq0, 0x08, 0x8E);

/* One IDT entry describes one interrupt/exception handler */
struct idt_entry {
    uint16_t base_low;   /* bits  0-15 of handler address */
    uint16_t selector;   /* GDT code segment selector     */
    uint8_t  reserved;   /* always zero (was "always0")   */
    uint8_t  flags;      /* type, DPL, and present bit    */
    uint16_t base_high;  /* bits 16-31 of handler address */
} __attribute__((packed));

/* Loaded into the CPU's IDTR register via `lidt` */
struct idt_ptr {
    uint16_t limit;      /* byte length of IDT minus 1 */
    uint32_t base;       /* linear address of IDT[0]   */
} __attribute__((packed));

/* -----------------------------------------------------------------------
 * Module-private state
 * ----------------------------------------------------------------------- */

static struct idt_entry idt[IDT_ENTRIES];
static struct idt_ptr   idtp;

/* -----------------------------------------------------------------------
 * Internal helpers
 * ----------------------------------------------------------------------- */

void idt_set_gate(uint8_t num, isr_t handler,
                         uint16_t sel, uint8_t flags)
{
    uint32_t base = (uint32_t)handler;

    idt[num].base_low  = (uint16_t)(base & 0xFFFF);
    idt[num].base_high = (uint16_t)(base >> 16);
    idt[num].selector  = sel;
    idt[num].reserved  = 0;
    idt[num].flags     = flags;
}

/* Minimal stub: swallows any unhandled interrupt and returns.
 * Marked naked so we emit a bare `iret` rather than a C epilogue that
 * would corrupt the interrupt frame on the stack. */
__attribute__((naked))
static void unhandled_isr(void)
{
    __asm__ __volatile__("iret");
}

/* -----------------------------------------------------------------------
 * Public API
 * ----------------------------------------------------------------------- */

void idt_install(void)
{
    idtp.limit = (uint16_t)(sizeof(idt) - 1);
    idtp.base  = (uint32_t)idt;

    for (int i = 0; i < IDT_ENTRIES; i++)
        idt_set_gate((uint8_t)i, unhandled_isr, KERNEL_CS, IDT_FLAG_KERNEL_GATE);

    __asm__ __volatile__("lidt %0" : : "m"(idtp) : "memory");
}

/* Register a custom handler for interrupt vector `num`.
 * Call this after idt_install() to override the stub. */
void idt_register_handler(uint8_t num, isr_t handler, uint8_t flags)
{
    idt_set_gate(num, handler, KERNEL_CS, flags);
}