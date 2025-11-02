#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    GPIO_MODE_INPUT,
    GPIO_MODE_OUTPUT
} gpio_mode_t;

typedef enum {
    GPIO_PULL_NONE,
    GPIO_PULL_UP,
    GPIO_PULL_DOWN
} gpio_pull_t;

void gpio_init(uint8_t pin, gpio_mode_t mode);
void gpio_write(uint8_t pin, uint8_t value);
uint8_t gpio_read(uint8_t pin);

void gpio_toggle(uint8_t pin);
void gpio_set_pull(uint8_t, gpio_pull_t pull);

//void gpio_set_function(uint8_t pin, gpio_func_t function);

#endif