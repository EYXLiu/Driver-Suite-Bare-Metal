#ifndef SYSTEM_H
#define SYSTEM_H

#include "pico/stdlib.h"

void system_config(void);

void system_enable_peripheral(uint peripheral_id, bool enable);
void system_reset_peripheral(uint peripheral_id);

void system_set_cpu_freq(uint freq_hz);

void system_enable_watchdog(uint timeout_ms);
void system_disable_watchdog(void);

#endif