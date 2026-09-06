#pragma once
#include <stdint.h>

// This struct represents everything the CPU pushed onto the stack
// when the interrupt fired, plus what our asm stub pushed
struct registers {
    // pushed by our asm stubs (in reverse order)
    uint32_t ds;                                     // data segment
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // general registers
    uint32_t int_no;                                 // interrupt number
    uint32_t err_code;                               // error code (0 if none)
    // pushed automatically by the CPU
    uint32_t eip, cs, eflags, useresp, ss;
};

void isr_install(void);
void isr_handler(struct registers *r);