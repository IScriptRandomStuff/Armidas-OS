#ifndef MEM_REGION_H
#define MEM_REGION_H

#include <stdint.h>

typedef struct {
    uint32_t addr;
    uint32_t len;
} mem_region_t;

#endif