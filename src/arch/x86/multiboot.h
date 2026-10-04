#pragma once
#include <stdint.h>

typedef struct {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length;
    uint32_t mmap_addr;
} multiboot_info_t;

typedef struct {
    uint32_t size;   // size of this entry, NOT counting this field
    uint64_t addr;   // 64-bit — must be uint64_t per the spec
    uint64_t len;    // 64-bit — must be uint64_t per the spec
    uint32_t type;   // 1 = usable
} __attribute__((packed)) mmap_entry_t;

#define MULTIBOOT_MEMORY_AVAILABLE 1