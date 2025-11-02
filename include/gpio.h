#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include <stdbool.h>

#define IO_BANK0_BASE ((uintptr_t)0x40014000)
#define SIO_BASE 0xD0000000u

#define GPIO_CTRL(n) (*(volatile uint32_t*)(IO_BANK0_BASE + ((n) * 0x8) + 0x4))
#define GPIO_FUNC_SIO 5u

#define SIO_GPIO_OUT (*(volatile uint32_t*)(SIO_BASE + 0x10))
#define SIO_GPIO_OUT_XOR (*(volatile uint32_t*)(SIO_BASE + 0x1C))
#define SIO_GPIO_IN (*(volatile uint32_t*)(SIO_BASE + 0x04))
#define SIO_GPIO_OE (*(volatile uint32_t*)(SIO_BASE + 0x20))

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
    GPIO_FUNC_GPIO, 
    GPIO_FUNC_ALT,
    GPIO_FUNC_UART
} gpio_func_t;

void gpio_init(uint8_t pin, gpio_mode_t mode);
void gpio_write(uint8_t pin, gpio_value_t value);
uint8_t gpio_read(uint8_t pin);

void gpio_toggle(uint8_t pin);
void gpio_set_pull(uint8_t, gpio_pull_t pull);

void gpio_set_func(uint8_t pin, gpio_func_t func);

#endif