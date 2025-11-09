#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include <stdbool.h>

void sleep_ms(uint64_t ms);
void sleep_us(uint64_t us);

uint64_t timer_get_ms(void);
uint64_t timer_get_us(void);

uint64_t timer_elapsed_us(uint64_t start);
bool timer_expired_us(uint64_t start, uint64_t duration);

#endif