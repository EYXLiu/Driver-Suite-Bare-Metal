#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#include <stddef.h>

typedef struct adc_inst_t adc_t;

adc_t *adc_init(uint8_t channel);

uint16_t adc_read_value(adc_t *adc);
uint16_t adc_read_avg(adc_t *adc, uint8_t samples);
float adc_read_voltage(adc_t *adc, float vref);

float adc_read_temp(adc_t *adc);
float adc_read_vref(adc_t *adc);

void adc_free(adc_t *adc);

#endif