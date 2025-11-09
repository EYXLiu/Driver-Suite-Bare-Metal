#include "adc.h"
#include "gpio.h"
#include "addressmap.h"
#include <stdlib.h>
#include <stdint.h>

#define ADC_CS (*(volatile uint32_t *)(ADC_BASE + 0x00))
#define ADC_RESULT (*(volatile uint32_t *)(ADC_BASE + 0x04))
#define ADC_DIV (*(volatile uint32_t *)(ADC_BASE + 0x10))

#define ADC_CS_START_ONCE (1 << 2)
#define ADC_CS_READY (1 << 3)
#define ADC_CS_AINSEL_MASK 0xF

struct adc_inst_t {
    uint8_t channel;
};

adc_t *adc_init(uint8_t channel) {
    if (channel > 4) return NULL;

    adc_t *adc = malloc(sizeof(adc));
    if (!adc) return NULL;

    adc->channel = channel;

    ADC_DIV = 1;

    if (channel <= 2) {
        uint8_t pin = 26 + channel;
        gpio_set_func(pin, GPIO_FUNC_XPI);
    }

    return adc;
}

uint16_t adc_read_value(adc_t *adc) {
    ADC_CS = (ADC_CS & ~ADC_CS_AINSEL_MASK) | (adc->channel & 0xF);
    ADC_CS |= ADC_CS_START_ONCE;

    while (!(ADC_CS & ADC_CS_READY));

    return ADC_RESULT & 0xFFF;
}

uint16_t adc_read_avg(adc_t *adc, uint8_t samples) {
    uint32_t sum = 0;
    for (uint8_t i = 0; i < samples; i++) {
        sum += adc_read_value(adc);
    }
    return sum / samples;
}

float adc_read_voltage(adc_t *adc, float vref) {
    uint16_t val = adc_read_value(adc);
    return (val / 4095.0f) * vref;
}

float adc_read_temp(adc_t *adc) {
    adc->channel = 3;
    uint16_t val = adc_read_value(adc);
    return 27.0f - ((val - 0.706f*4095) / 0.001271f);
}

float adc_read_ref(adc_t *adc) {
    adc->channel = 4;
    uint16_t val = adc_read_value(adc);
    return (val / 4095.0f) * 3.3f;
}

void adc_free(adc_t *adc) {
    if (!adc) return;
    free(adc);
}
