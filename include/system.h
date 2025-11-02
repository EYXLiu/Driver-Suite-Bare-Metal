#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdint.h>
#include <stdbool.h>

void system_config(void);

void system_enable_peripheral(uint32_t peripheral_id, bool enable);
void system_reset_peripheral(uint32_t peripheral_id);

void system_set_cpu_freq(uint32_t freq_hz);

void system_enable_watchdog(uint32_t timeout_ms);
void system_disable_watchdog(void);

#endif