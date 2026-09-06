#include "../drivers/vga.h"
#include "../arch/x86/gdt.h"
#include "../arch/x86/idt.h"
#include "../arch/x86/isr.h"
#include "../arch/x86/irq.h"

void kernel_main(void)
{
    // Bootstrap
    vga_clear();
    vga_print("VGA PDD Booted\n");
    gdt_install();
    vga_print("GDT Installed\n");
    idt_install();
    vga_print("IDT Installed\n");
    isr_install();
    vga_print("ISR Installed\n");
    irq_install();
    __asm__ __volatile__("sti");
    vga_print("IRQs Enabled\n");

    vga_print("Bootstrap Success!\n");

    // Continuation Loop
    for (;;) {
        __asm__ volatile ("hlt");
    }
}