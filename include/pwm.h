#ifndef PWM_H
#define PWM_H

#include <stdint.h>
#include <stdbool.h>

typedef struct pwm_inst_t pwm_t;

pwm_t *pwm_config_init(uint8_t pin, uint32_t freq_hz);

uint32_t pwm_get_wrap(pwm_t *pwm);

void pwm_set_freq(pwm_t *pwm, uint32_t freq_hz);

float pwm_get_duty(pwm_t *pwm);
void pwm_set_duty (pwm_t *pwm, float duty_percent);

void pwm_enable_pin(pwm_t *pwm);
void pwm_disable_pin(pwm_t *pwm);

void pwm_free(pwm_t *pwm);

#endif