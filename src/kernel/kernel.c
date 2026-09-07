#include "../drivers/vga.h"
#include "../arch/x86/gdt.h"
#include "../arch/x86/idt.h"
#include "../arch/x86/isr.h"
#include "../arch/x86/irq.h"
#include "../arch/x86/timer.h"
#include "../arch/x86/keyboard.h"

static void on_key(char c)
{
    vga_putchar(c);
}

void kernel_main(void)
{
    // Bootstrap
    vga_clear();
    vga_print("VGA PDD Boot\n");
    gdt_install();
    vga_print("GDT Inst\n");
    idt_install();
    vga_print("IDT Inst\n");
    isr_install();
    vga_print("ISR Inst\n");
    irq_install();
    vga_print("IRQ Inst\n");
    timer_install(1000);
    vga_print("Timer Inst\n");
    keyboard_install();
    keyboard_on_keypress(on_key);
    vga_print("Keyboard Inst\n");

    __asm__ __volatile__("sti");
    vga_print("Bootstrap Success!\n");

    // Continuation Loop
    for (;;) {
        __asm__ volatile ("hlt");
    }
}