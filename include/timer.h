#ifndef TIMER_H
#define TIMER_H

#include "pico/stdlib.h"

void timer_delay_ms(uint32_t ms);
void timer_delay_us(uint32_t us);

uint64_t timer_get_ms(void);
uint64_t timer_get_us(void);

uint64_t timer_elapsed_us(uint64_t start);
bool timer_expired_us(uint64_t start, uint64_t duration);

#endif