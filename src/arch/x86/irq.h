#pragma once
#include <stdint.h>
#include "isr.h"

void irq_install(void);
void irq_install_handler(uint8_t irq, void (*handler)(struct registers *r));
void pic_send_eoi(uint8_t irq);

// stubs
extern void irq0(void);  extern void irq1(void);
extern void irq2(void);  extern void irq3(void);
extern void irq4(void);  extern void irq5(void);
extern void irq6(void);  extern void irq7(void);
extern void irq8(void);  extern void irq9(void);
extern void irq10(void); extern void irq11(void);
extern void irq12(void); extern void irq13(void);
extern void irq14(void); extern void irq15(void);