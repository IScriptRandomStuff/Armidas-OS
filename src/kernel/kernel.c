#include "../drivers/vga.h"
#include "../arch/x86/gdt.h"

void kernel_main(void)
{
    // VGA
    vga_clear();
    vga_print("VGA Driver Booted!\n");
        gdt_install();

    // Continuation Loop
    for (;;) {
        __asm__ volatile ("hlt");
    }
}