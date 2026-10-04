#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "heap.h"
#include "pmm.h"
#include "vmm.h"
#include "drivers/vga.h"    // for heap_dump

typedef struct heap_block {
    uint32_t          magic;    // sanity check — detects heap corruption
    uint32_t          size;     // size of usable bytes (NOT including header)
    uint8_t           free;     // 1 = free, 0 = in use
    struct heap_block *next;    // next block in the list
    struct heap_block *prev;    // previous block in the list
} heap_block_t;

#define HEAP_MAGIC   0xDEADBEEF   // magic value — if this is wrong, heap is corrupt
#define HEADER_SIZE  sizeof(heap_block_t)

static heap_block_t *heap_head = 0;
static uint32_t      heap_top  = 0;

// ── private helpers ───────────────────────────────────────────────────────────

// map enough physical pages to cover [start, start+size)
static void map_heap_pages(uint32_t start, uint32_t size) {
    uint32_t addr = start & ~(PAGE_SIZE - 1);
    uint32_t end  = (start + size + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);

    while (addr < end) {
        void *page = pmm_alloc_page();
        vmm_map(addr, (uint32_t)page, VMM_PRESENT | VMM_WRITABLE);
        addr += PAGE_SIZE;
    }
}

// merge adjacent free blocks so we don't fragment
// think of it like combining two side-by-side empty lockers into one big one
static void coalesce(heap_block_t *block) {
    // merge with next block if it's free
    if (block->next && block->next->free) {
        block->size += HEADER_SIZE + block->next->size;  // absorb header too
        block->next  = block->next->next;
        if (block->next)
            block->next->prev = block;
    }

    // merge with previous block if it's free
    if (block->prev && block->prev->free) {
        block->prev->size += HEADER_SIZE + block->size;
        block->prev->next  = block->next;
        if (block->next)
            block->next->prev = block->prev;
    }
}

// split a block if it's big enough to share
// like dividing one big locker into two smaller ones
static void split(heap_block_t *block, size_t size) {
    // only split if the leftover would fit a header + at least 16 bytes
    if (block->size < size + HEADER_SIZE + 16)
        return;

    // carve a new block out of the tail end
    heap_block_t *newblock = (heap_block_t *)((uint8_t *)block + HEADER_SIZE + size);
    newblock->magic = HEAP_MAGIC;
    newblock->size  = block->size - size - HEADER_SIZE;
    newblock->free  = 1;
    newblock->next  = block->next;
    newblock->prev  = block;

    if (block->next)
        block->next->prev = newblock;

    block->next = newblock;
    block->size = size;
}

// ── public API ────────────────────────────────────────────────────────────────

void heap_init(void) {
    // map the initial heap pages into virtual memory
    map_heap_pages(HEAP_START, HEAP_INITIAL);

    // set up the first block covering all of it
    heap_head        = (heap_block_t *)HEAP_START;
    heap_head->magic = HEAP_MAGIC;
    heap_head->size  = HEAP_INITIAL - HEADER_SIZE;
    heap_head->free  = 1;
    heap_head->next  = 0;
    heap_head->prev  = 0;
}

void *kmalloc(size_t size) {
    if (!size) return 0;

    // align size to 4 bytes so headers stay aligned
    size = (size + 3) & ~3;

    heap_block_t *curr = heap_head;

    // walk the list looking for a free block that fits
    while (curr) {
        if (curr->magic != HEAP_MAGIC) {
            vga_print("kmalloc: heap corruption detected!\n");
            return 0;
        }

        if (curr->free && curr->size >= size) {
            split(curr, size);       // split if there's leftover space
            curr->free = 0;
            // return pointer PAST the header to the usable bytes
            return (void *)((uint8_t *)curr + HEADER_SIZE);
        }

        curr = curr->next;
    }

    // Expansion Block
    if (heap_top + PAGE_SIZE > HEAP_MAX) {
        vga_print("kmalloc: heap ceiling reached!\n");
        return 0;
    }
    uint32_t new_phys = (uint32_t)pmm_alloc_page();
    if (!new_phys) {
        vga_print("kmalloc: out of physical memory!\n");
        return 0;
    }

    // map the new page at wherever the heap currently ends
    vmm_map(heap_top, new_phys, VMM_PRESENT | VMM_WRITABLE);

    heap_block_t *newblock = (heap_block_t *)heap_top;
    newblock->magic = HEAP_MAGIC;
    newblock->size  = PAGE_SIZE - HEADER_SIZE;
    newblock->free  = 1;
    newblock->next  = 0;

    // move heap_top forward
    heap_top += PAGE_SIZE;

    // attach to end of list
    heap_block_t *tail = heap_head;
    while (tail->next) tail = tail->next;
    newblock->prev = tail;
    tail->next     = newblock;

    // try again now that there's space
    return kmalloc(size);
}

void kfree(void *ptr) {
    if (!ptr) return;

    // step back past the usable bytes to find the header
    // like reading the sticky note on the locker
    heap_block_t *block = (heap_block_t *)((uint8_t *)ptr - HEADER_SIZE);

    if (block->magic != HEAP_MAGIC) {
        vga_print("kfree: invalid or corrupt pointer!\n");
        return;
    }

    if (block->free) {
        vga_print("kfree: double free detected!\n");
        return;
    }

    block->free = 1;
    coalesce(block);   // merge neighbours — keep the hallway tidy
}

// ── debug ─────────────────────────────────────────────────────────────────────

void heap_dump(void) {
    heap_block_t *curr = heap_head;
    vga_print("── heap dump ──\n");
    while (curr) {
        if (curr->magic != HEAP_MAGIC) {
            vga_print("  [CORRUPT BLOCK]\n");
            break;
        }
        // you can expand this to print size/free with your vga int printer
        vga_print(curr->free ? "  [FREE]  " : "  [USED]  ");
        vga_print("\n");
        curr = curr->next;
    }
}