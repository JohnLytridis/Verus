#include "pit.h"
#include "io.h"

#define PIT_HZ 100

static volatile unsigned int ticks = 0;

void pit_init(void) {
    unsigned int divisor = 1193182 / PIT_HZ;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
}

void timer_handler(void) {
    ticks++;
    outb(0x20, 0x20);
}

unsigned int pit_ticks(void) {
    return ticks;
}

unsigned int pit_seconds(void) {
    return ticks / PIT_HZ;
}