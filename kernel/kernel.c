#include "vga.h"

void kernel_main(void)
{
    // VGA
    vga_clear();
    vga_print("VGA Driver Booted!\n");

    // Continuation Loop
    for (;;) {
        __asm__ volatile ("hlt");
    }
}