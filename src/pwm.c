#include "pwm.h"
#include "gpio.h"
#include "addressmap.h"
#include <stdlib.h>
#include <stdint.h>

#define PWM_CHn_CSR(n) (*(volatile uint32_t *)(PWM_BASE + 0x00 + (n)*0x10))
#define PWM_CHn_DIV(n) (*(volatile uint32_t *)(PWM_BASE + 0x04 + (n)*0x10))
#define PWM_CHn_CC(n) (*(volatile uint32_t *)(PWM_BASE + 0x0C + (n)*0x10))
#define PWM_CHn_TOP(n) (*(volatile uint32_t *)(PWM_BASE + 0x10 + (n)*0x10))

#define PWM_CH_CSR_EN (1 << 0)
#define PWM_CH_CSR_A_ENABLE (1 << 4)
#define PWM_CH_CSR_B_ENABLE (1 << 5)

#define PWM_DIV_INT_SHIFT 4
#define PWM_DIV_FRAC_MASK 0xF

#define PWM_CLK 12000000u

struct pwm_inst_t {
    uint8_t pin;
    uint8_t slice;
    uint8_t channel;
    uint32_t top;
};

uint8_t pwm_gpio_to_slice(uint8_t pin) {
    return pin / 2;
}

uint8_t pwm_gpio_to_channel(uint8_t pin) {
    return pin % 2;
}

pwm_t *pwm_init(uint8_t pin, uint32_t freq_hz) {
    pwm_t *pwm = malloc(sizeof(pwm_t));
    if (!pwm) return NULL;

    pwm->pin = pin;
    pwm->slice = pwm_gpio_to_slice(pin);
    pwm->channel = pwm_gpio_to_channel(pin);

    gpio_set_func(pin, GPIO_FUNC_PWM);

    PWM_CHn_CSR(pwm->slice) &= ~PWM_CH_CSR_EN;

    pwm->top = 255;
    PWM_CHn_TOP(pwm->slice) = pwm->top;
    
    uint32_t div = (PWM_CLK / ((pwm->top + 1) * freq_hz));
    if (div == 0) div = 1;
    PWM_CHn_DIV(pwm->slice) = div << 4;

    if (pwm->channel == 0)
        PWM_CHn_CSR(pwm->slice) |= PWM_CH_CSR_A_ENABLE;
    else
        PWM_CHn_CSR(pwm->slice) |= PWM_CH_CSR_B_ENABLE;
    
    PWM_CHn_CSR(pwm->slice) |= PWM_CH_CSR_EN;

    return pwm;
}

uint32_t pwm_get_wrap(pwm_t *pwm) {
    return PWM_CHn_TOP(pwm->slice);
}

void pwm_set_freq(pwm_t *pwm, uint32_t freq_hz) {
    PWM_CHn_CSR(pwm->slice) &= ~PWM_CH_CSR_EN;

    uint32_t div = (PWM_CLK / ((pwm->top + 1) * freq_hz));
    if (div == 0) div = 1;
    PWM_CHn_DIV(pwm->slice) = div << 4;

    PWM_CHn_CSR(pwm->slice) |= PWM_CH_CSR_EN;
}

float pwm_get_duty(pwm_t *pwm) {
    uint32_t cc = PWM_CHn_CC(pwm->slice);
    uint32_t compare = (pwm->channel == 0) ? (cc & 0xFFFF) : (cc >> 16);
    return (float)compare / (pwm->top + 1) * 100.0f;
}

void pwm_set_duty (pwm_t *pwm, float duty_percent) {
    if (duty_percent > 100.0f) duty_percent = 100.0f;
    uint32_t compare = (uint32_t)((duty_percent / 100.0f) * (pwm->top + 1));
    uint32_t cc = PWM_CHn_CC(pwm->slice);
    if (pwm->channel == 0)
        cc = (cc & 0xFFFF0000) | (compare & 0xFFFF);
    else
        cc = (cc & 0x0000FFFF) | ((compare & 0xFFFF) << 16);
    
    PWM_CHn_CC(pwm->slice) = cc;
}

void pwm_enable_pin(pwm_t *pwm) {
    if (pwm->channel == 0)
        PWM_CHn_CSR(pwm->slice) |= PWM_CH_CSR_A_ENABLE;
    else
        PWM_CHn_CSR(pwm->slice) |= PWM_CH_CSR_B_ENABLE;
}

void pwm_disable_pin(pwm_t *pwm) {
    if (pwm->channel == 0)
        PWM_CHn_CSR(pwm->slice) &= ~PWM_CH_CSR_A_ENABLE;
    else
        PWM_CHn_CSR(pwm->slice) &= ~PWM_CH_CSR_B_ENABLE;
}

void pwm_free(pwm_t *pwm) {
    if (!pwm) return;
    PWM_CHn_CSR(pwm->slice) &= ~PWM_CH_CSR_EN;
    free(pwm);
}
