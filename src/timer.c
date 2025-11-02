#include "timer.h"

#define TIMER_BASE 0x40054000u
#define TIMER_TIMERAWL (*(volatile uint32_t*)(TIMER_BASE + 0x0))
#define TIMER_TIMERAWH (*(volatile uint32_t*)(TIMER_BASE + 0x4))

void sleep_ms(uint32_t ms) {
    sleep_us(ms * 1000);
}

void sleep_us(uint32_t us) {
    uint64_t start = timer_get_us();
    while ((timer_get_us() - start) < us);
}

uint64_t timer_get_ms(void) {
    uint32_t hi, lo1, lo2;
    do {
        hi = TIMER_TIMERAWH;
        lo1 = TIMER_TIMERAWL;
        lo2 = TIMER_TIMERAWL;
    } while (lo2 < lo1);

    return ((uint64_t)hi << 32) | lo2;
}
uint64_t timer_get_us(void) {
    return timer_get_ms() / 1000;
}

uint64_t timer_elapsed_us(uint64_t start) {
    return timer_get_us() - start;
}

bool timer_expired_us(uint64_t start, uint64_t duration) {
    return timer_elapsed_us(start) >= duration;
}