#include "timer.h"
#include "io.h"

#define PIT_COMMAND 0x43
#define PIT_CHANNEL0 0x40

#define PIT_FREQUENCY 1193182
#define TIMER_FREQUENCY 100

static volatile uint64_t ticks = 0;

void timer_handler(void)
{
    ticks++;
}

uint64_t timer_get_ticks(void)
{
    return ticks;
}

void timer_init(void)
{
    uint16_t divisor;

    divisor = PIT_FREQUENCY / TIMER_FREQUENCY;

    /*
     * Channel 0
     * Access mode: low byte followed by high byte
     * Mode 3: square wave generator
     */
    outb(PIT_COMMAND, 0x36);

    outb(PIT_CHANNEL0, divisor & 0xFF);
    outb(PIT_CHANNEL0, (divisor >> 8) & 0xFF);
}