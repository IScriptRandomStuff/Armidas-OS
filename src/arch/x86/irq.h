#pragma once
#include <stdint.h>
#include "isr.h"

void irq_install(void);
void irq_install_handler(uint8_t irq, void (*handler)(struct registers *r));
void pic_send_eoi(uint8_t irq);