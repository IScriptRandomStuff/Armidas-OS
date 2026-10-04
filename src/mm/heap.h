#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>
#include <stddef.h>     // for size_t

// where the kernel heap starts in virtual memory
// sits just above the first 4MB identity map
#define HEAP_START    0x00400000
#define HEAP_INITIAL  0x00100000    // start with 1MB (grows as needed)
#define HEAP_MAX  0x00800000

void  heap_init(void);
void *kmalloc(size_t size);
void  kfree(void *ptr);

// debug helper — prints heap state via vga_print
void  heap_dump(void);

#endif