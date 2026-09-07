#pragma once
#include <stdint.h>

void     timer_install(uint32_t hz);
uint32_t timer_get_ticks(void);
void     timer_sleep_ms(uint32_t ms);