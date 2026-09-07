#include <stdint.h>
#include "timer.h"
#include "io.h"
#include "irq.h"
#include "isr.h"

#define PIT_COMMAND  0x43
#define PIT_CHANNEL0 0x40
static volatile uint32_t tick_count = 0;
static uint32_t timer_hz = 0;

static void timer_callback(struct registers *r)
{
    (void)r; // silence unused parameter warning
    tick_count++;
}

void timer_install(uint32_t hz)
{
    timer_hz = hz;
    uint16_t divisor = 1193182 / hz;

    outb(0x43, 0x36);   io_wait();
    outb(0x40, divisor & 0xFF);   io_wait();
    outb(0x40, divisor >> 8);   io_wait();

    irq_install_handler(0, timer_callback);
}

uint32_t timer_get_ticks(void)
{
    return tick_count;
}

void timer_sleep_ms(uint32_t ms)
{
    uint32_t target  = tick_count + (timer_hz * ms / 1000);
    while (tick_count < target)
    {
        __asm__ __volatile__("hlt");
    }
}