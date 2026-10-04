// Arch
#include "../arch/x86/gdt.h"
#include "../arch/x86/idt.h"
#include "../arch/x86/isr.h"
#include "../arch/x86/irq.h"
#include "../arch/x86/timer.h"
#include "../arch/x86/keyboard.h"

// MM & Multiboot
#include <multiboot.h>
#include "../mm/pmm.h"
#include "../mm/vmm.h"
#include "../mm/heap.h"

// Drivers (non-arch specifc)
#include "../drivers/vga.h"

#define MAX_REGIONS 32

static void on_key(char c)
{
    vga_putchar(c);
}

void kernel_main(multiboot_info_t* mb, uint32_t magic)
{
    // Bootstrap
    vga_clear();
    vga_print("VGA PDD Boot\n");
    if (magic != 0x2BADB002) {
        vga_clear();
        vga_print("Invalid Multiboot Magic");
        vga_print_hex(magic);
        for (;;) __asm__ __volatile__("hlt");
    }
    if (!(mb->flags & MB_FLAG_MMAP)) {
        vga_clear();
        vga_print("No Bootloader Memory Map\n");
        for (;;) __asm__ __volatile__("hlt");
    }

    gdt_install();
    vga_print("GDT Inst\n");
    idt_install();
    vga_print("IDT Inst\n");
    isr_install();
    vga_print("ISR Inst\n");
    irq_install();
    vga_print("IRQ Inst\n");

    pmm_install(mb);
    vga_print("PMM Inst\n");
    vmm_init(mb);
    vga_print("VMM Init\n");
    heap_init();
    vga_print("Heap Init\n");

    timer_install(1000);
    vga_print("Timer Inst\n");
    keyboard_install();
    keyboard_on_keypress(on_key);
    vga_print("Keyboard Inst\n");

    // Sti
    __asm__ __volatile__("sti");
    vga_print("Bootstrap Success!\n");

    // Continuation Loop
    for (;;) {
        __asm__ volatile ("hlt");
    }
}