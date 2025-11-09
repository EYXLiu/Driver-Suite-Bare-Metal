#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    GPIO_MODE_INPUT,
    GPIO_MODE_OUTPUT
} gpio_mode_t;

typedef enum {
    GPIO_LOW = 0,
    GPIO_HIGH = 1
} gpio_value_t;

typedef enum {
    GPIO_PULL_NONE,
    GPIO_PULL_UP,
    GPIO_PULL_DOWN
} gpio_pull_t;

typedef enum {
    GPIO_FUNC_XPI = 0,
    GPIO_FUNC_UART = 1, 
    GPIO_FUNC_SPI = 2,
    GPIO_FUNC_I2C = 3,
    GPIO_FUNC_PWM = 4,
    GPIO_FUNC_SIO = 5
} gpio_func_t;

typedef struct gpio_inst_t gpio_t;

gpio_t *gpio_init(uint8_t pin, gpio_mode_t mode);
void gpio_write(gpio_t *gpio, gpio_value_t value);
void gpio_write_pin(uint8_t pin, gpio_value_t value);
uint8_t gpio_read(gpio_t *gpio);
uint8_t gpio_read_pin(uint8_t pin);

void gpio_toggle(gpio_t *gpio);
void gpio_set_pull(gpio_t *gpio, gpio_pull_t pull);

void gpio_set_func(uint8_t pin, gpio_func_t func);
gpio_func_t gpio_get_func(uint8_t pin);

void gpio_free(gpio_t *gpio);

#endif