#include "../drivers/vga.h"
#include "../arch/x86/gdt.h"
#include "../arch/x86/idt.h"

void kernel_main(void)
{
    // VGA
    vga_clear();
    vga_print("VGA Driver Booted!\n");
    gdt_install();
    vga_print("GDT Installed!\n");
    idt_install();
    vga_print("IDT Installed!\n");

    // Continuation Loop
    for (;;) {
        __asm__ volatile ("hlt");
    }
}