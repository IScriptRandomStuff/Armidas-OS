#include <stdint.h>
#include <string.h>
#include "multiboot.h"
#include "pmm.h"

static uint32_t bitmap[MAX_PAGES / 32];

// defined in linker.ld
extern uint32_t _kernel_start;
extern uint32_t _kernel_end;

// ── private helpers ──────────────────────────────────────────────────────────

static void _set_page(uint32_t page) {
    bitmap[page / 32] |=  (1u << (page % 32));
}

static void _clr_page(uint32_t page) {
    bitmap[page / 32] &= ~(1u << (page % 32));
}

// ── public API ───────────────────────────────────────────────────────────────

void pmm_init(mem_region_t *regions, uint32_t count) {
    // start with every page reserved
    memset(bitmap, 0xFF, sizeof(bitmap));

    // free pages that the memory map says are usable
    for (uint32_t r = 0; r < count; r++) {
        uint32_t start = regions[r].addr;
        uint32_t end   = start + regions[r].len;

        // align start up, end down to page boundaries
        start = (start + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
        end   =  end                    & ~(PAGE_SIZE - 1);

        for (uint32_t addr = start; addr < end; addr += PAGE_SIZE)
            _clr_page(addr / PAGE_SIZE);
    }

    // re-reserve the low 1 MB (BIOS, IVT, VGA, etc.)
    uint32_t low_end = (0x100000 + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
    for (uint32_t addr = 0; addr < low_end; addr += PAGE_SIZE)
        _set_page(addr / PAGE_SIZE);

    // re-reserve the kernel image itself
    uint32_t k_start = (uint32_t)&_kernel_start & ~(PAGE_SIZE - 1);
    uint32_t k_end   = ((uint32_t)&_kernel_end + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
    for (uint32_t addr = k_start; addr < k_end; addr += PAGE_SIZE)
        _set_page(addr / PAGE_SIZE);

    // re-reserve the bitmap itself (it's in .bss, so already covered above,
    // but be explicit in case you ever move it)
    uint32_t bm_start = (uint32_t)bitmap & ~(PAGE_SIZE - 1);
    uint32_t bm_end   = ((uint32_t)bitmap + sizeof(bitmap) + PAGE_SIZE - 1)
                        & ~(PAGE_SIZE - 1);
    for (uint32_t addr = bm_start; addr < bm_end; addr += PAGE_SIZE)
        _set_page(addr / PAGE_SIZE);
}

void pmm_install(multiboot_info_t *mb) {
    mem_region_t regions[MAX_REGIONS];
    uint32_t     count = 0;

    if (mb->flags & (1 << 6)) {          // mmap fields are valid
        multiboot_mmap_entry_t *entry =
            (multiboot_mmap_entry_t *)mb->mmap_addr;
        multiboot_mmap_entry_t *end   =
            (multiboot_mmap_entry_t *)(mb->mmap_addr + mb->mmap_length);

        while (entry < end && count < MAX_REGIONS) {
            if (entry->type == MULTIBOOT_MEMORY_AVAILABLE) {
                // safe cast: we only track 32-bit physical space here
                regions[count].addr = (uint32_t)entry->addr;
                regions[count].len  = (uint32_t)entry->len;
                count++;
            }
            // entry->size does NOT include the size field itself
            entry = (multiboot_mmap_entry_t *)
                    ((uint32_t)entry + entry->size + sizeof(entry->size));
        }
    }

    pmm_init(regions, count);
}

void *pmm_alloc_page(void) {
    for (uint32_t i = 0; i < MAX_PAGES / 32; i++) {
        if (bitmap[i] == 0xFFFFFFFF) continue;
        for (uint32_t b = 0; b < 32; b++) {
            if (!(bitmap[i] & (1u << b))) {
                bitmap[i] |= (1u << b);
                return (void *)((i * 32 + b) * PAGE_SIZE);
            }
        }
    }
    return 0; // ramageddon
}

void pmm_free_page(void *addr) {
    uint32_t page = (uint32_t)addr / PAGE_SIZE;
    _clr_page(page);
}

void pmm_mark_page(void *addr) {
    uint32_t page = (uint32_t)addr / PAGE_SIZE;
    _set_page(page);
}