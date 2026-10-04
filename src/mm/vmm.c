#include <stdint.h>
#include <string.h>
#include "vmm.h"
#include "pmm.h"

// the kernel's page directory — 4KB aligned
static pde_t page_directory[PD_ENTRIES] __attribute__((aligned(4096)));

// ── private helpers ──────────────────────────────────────────────────────────

// get (or create) the page table for a given virtual address
static pte_t *get_or_create_pt(uint32_t va) {
    uint32_t pdi = PD_INDEX(va);

    if (page_directory[pdi] & VMM_PRESENT) {
        // PT already exists — strip flag bits to get physical addr
        return (pte_t *)(page_directory[pdi] & ~0xFFF);
    }

    // allocate a fresh page for the new PT
    pte_t *pt = (pte_t *)pmm_alloc_page();
    if (!pt) return 0;   // out of memory

    memset(pt, 0, PAGE_SIZE);
    page_directory[pdi] = (uint32_t)pt | VMM_PRESENT | VMM_WRITABLE;
    return pt;
}

// ── public API ───────────────────────────────────────────────────────────────

void vmm_map(uint32_t va, uint32_t pa, uint32_t flags) {
    pte_t *pt = get_or_create_pt(va);
    if (!pt) return;

    pt[PT_INDEX(va)] = (pa & ~0xFFF) | (flags | VMM_PRESENT);

    // flush TLB for this page
    __asm__ __volatile__("invlpg (%0)" :: "r"(va) : "memory");
}

void vmm_unmap(uint32_t va) {
    uint32_t pdi = PD_INDEX(va);
    if (!(page_directory[pdi] & VMM_PRESENT)) return;

    pte_t *pt = (pte_t *)(page_directory[pdi] & ~0xFFF);
    pt[PT_INDEX(va)] = 0;

    __asm__ __volatile__("invlpg (%0)" :: "r"(va) : "memory");
}

int vmm_is_mapped(uint32_t va) {
    uint32_t pdi = PD_INDEX(va);
    if (!(page_directory[pdi] & VMM_PRESENT)) return 0;

    pte_t *pt = (pte_t *)(page_directory[pdi] & ~0xFFF);
    return (pt[PT_INDEX(va)] & VMM_PRESENT) ? 1 : 0;
}

void vmm_init(void) {
    // clear the page directory
    memset(page_directory, 0, sizeof(page_directory));

    // identity-map the first 4MB (kernel lives here at 0x100000)
    // this keeps the kernel accessible once paging is turned on
    for (uint32_t addr = 0; addr < 0x400000; addr += PAGE_SIZE)
        vmm_map(addr, addr, VMM_PRESENT | VMM_WRITABLE);

    // point CR3 at our page directory and enable paging in CR0
    __asm__ __volatile__(
        "mov %0, %%cr3\n"          // load page directory physical addr
        "mov %%cr0, %%eax\n"
        "or  $0x80000000, %%eax\n" // set PG bit
        "mov %%eax, %%cr0\n"
        :
        : "r"((uint32_t)page_directory)
        : "eax"
    );
}