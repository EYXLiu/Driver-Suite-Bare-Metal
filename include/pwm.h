#ifndef PWM_H
#define PWM_H

#include "pico/stdlib.h"

void pwm_config_init(uint pin, uint freq_hz);

uint16_t pwm_get_wrap(uint pin);

uint16_t pwm_get_chan_level_pin(uint pin);
uint16_t pwm_get_chan_level(uint slice_num, uint channel);

float pwm_get_clkdiv_pin(uint pin);
float pwm_get_clkdiv(uint slice_num);

void pwm_set_duty(uint pin, float duty_percent);
void pwm_get_duty(uint pin);

void pwm_set_freq(uint pin, uint freq_hz);

void pwm_enable_pin(uint pin);
void pwm_disable_pin(uint pin);

void pwm_set_raw(uint slice_num, uint16_t wrap, uint16_t level);

#endif