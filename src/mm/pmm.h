#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include "mem_region.h"
#include "multiboot.h"

#define PAGE_SIZE 4096
#define MAX_PAGES 32768

void pmm_init(mem_region_t* regions, uint32_t count);
void pmm_install(multiboot_info_t *mb);
void* pmm_alloc_page();
void  pmm_free_page(void* addr);
void  pmm_mark_page(void* addr);

#endif