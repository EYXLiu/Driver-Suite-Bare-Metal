#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#include <stddef.h>

void adc_config(uint32_t channel);
void adc_select_channel(uint32_t channel);

uint16_t adc_read_value(void);
uint16_t adc_read_channel(uint32_t channel);
uint16_t adc_read_avg(uint32_t channel, uint8_t samples);
float adc_read_voltage(uint32_t channel);

float adc_read_temp(void);
float adc_read_vref(void);

#endif