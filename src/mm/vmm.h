#ifndef VMM_H
#define VMM_H

#include <stdint.h>

// page flag bits (lower 12 bits of a PDE/PTE)
#define VMM_PRESENT    (1 << 0)   // page is mapped
#define VMM_WRITABLE   (1 << 1)   // page is writable
#define VMM_USER       (1 << 2)   // user-mode accessible

#define PAGE_SIZE      4096
#define PD_ENTRIES     1024
#define PT_ENTRIES     1024

// extract indices from a virtual address
#define PD_INDEX(va)   (((uint32_t)(va)) >> 22)
#define PT_INDEX(va)   ((((uint32_t)(va)) >> 12) & 0x3FF)
#define PAGE_ALIGN(a)  (((uint32_t)(a) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))

typedef uint32_t pde_t;   // page directory entry
typedef uint32_t pte_t;   // page table entry

void  vmm_init(void);
void  vmm_map(uint32_t va, uint32_t pa, uint32_t flags);
void  vmm_unmap(uint32_t va);
int   vmm_is_mapped(uint32_t va);

#endif