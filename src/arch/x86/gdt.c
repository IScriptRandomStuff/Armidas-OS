#include "gdt.h"

// A single GDT entry is 8 bytes and describes one memory segment
struct gdt_entry {
    uint16_t limit_low;   // lower 16 bits of the segment size
    uint16_t base_low;    // lower 16 bits of where segment starts
    uint8_t  base_mid;    // next 8 bits of base
    uint8_t  access;      // who can use this segment and how
    uint8_t  granularity; // upper 4 bits of limit + flags
    uint8_t  base_high;   // last 8 bits of base
} __attribute__((packed));

// This gets loaded into the CPU's GDTR register
struct gdt_ptr {
    uint16_t limit;   // size of the GDT minus 1
    uint32_t base;    // address of the GDT in memory
} __attribute__((packed));

// Our GDT has 3 entries: null, code, data
static struct gdt_entry gdt[3];
static struct gdt_ptr   gdtp;

// Defined in boot.asm — flushes the GDT into the CPU
extern void gdt_flush(uint32_t);

static void gdt_set(int i, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
    gdt[i].base_low    = (base & 0xFFFF);
    gdt[i].base_mid    = (base >> 16) & 0xFF;
    gdt[i].base_high   = (base >> 24) & 0xFF;
    gdt[i].limit_low   = (limit & 0xFFFF);
    gdt[i].granularity = ((limit >> 16) & 0x0F) | (gran & 0xF0);
    gdt[i].access      = access;
}

void gdt_install(void)
{
    gdtp.limit = (sizeof(struct gdt_entry) * 3) - 1;
    gdtp.base  = (uint32_t)&gdt;

    gdt_set(0, 0, 0, 0, 0);                // Null descriptor — required by CPU
    gdt_set(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
    gdt_set(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment

    gdt_flush((uint32_t)&gdtp);
}