#include "gpio.h"
#include "addressmap.h"
#include <stdlib.h>
#include <stdint.h>

#define GPIO_CTRL(n) (*(volatile uint32_t *)(IO_BANK0_BASE + ((n) * 0x8) + 0x4))
#define GPIO_PAD(n) (*(volatile uint32_t *)(PADS_BANK0_BASE + ((n) * 4)))

#define SIO_GPIO_OUT (*(volatile uint32_t*)(SIO_BASE + 0x10))
#define SIO_GPIO_OUT_XOR (*(volatile uint32_t*)(SIO_BASE + 0x1C))
#define SIO_GPIO_IN (*(volatile uint32_t*)(SIO_BASE + 0x04))
#define SIO_GPIO_OE (*(volatile uint32_t*)(SIO_BASE + 0x20))

struct gpio_inst_t {
    uint8_t pin;
    gpio_mode_t mode;
    gpio_pull_t pull;
};

gpio_t *gpio_init(uint8_t pin, gpio_mode_t mode) {
    gpio_t *gpio = malloc(sizeof(gpio_t));
    if (!gpio) return NULL;

    gpio->pin = pin;
    gpio->mode = mode;
    gpio->pull = GPIO_PULL_NONE;

    GPIO_CTRL(pin) = GPIO_FUNC_SIO;

    if (mode == GPIO_MODE_OUTPUT) {
        SIO_GPIO_OE |= (1u << pin);
    } else {
        SIO_GPIO_OE &= ~(1u << pin);
    }

    return gpio;
}

void gpio_write(gpio_t *gpio, gpio_value_t value) {
    if (value == GPIO_LOW) {
        SIO_GPIO_OUT |= (1u << gpio->pin);
    } else {
        SIO_GPIO_OUT &= ~(1u << gpio->pin);
    }
}

void gpio_write_pin(uint8_t pin, gpio_value_t value) {
    if (value == GPIO_LOW) {
        SIO_GPIO_OUT &= ~(1u << pin);
    } else {
        SIO_GPIO_OUT |= (1u << pin);
    }
}

uint8_t gpio_read(gpio_t *gpio) {
    return (SIO_GPIO_IN & (1u << gpio->pin)) != 0;
}

uint8_t gpio_read_pin(uint8_t pin) {
    return (SIO_GPIO_IN & (1u << pin)) != 0;
}

void gpio_toggle(gpio_t *gpio) {
    SIO_GPIO_OUT_XOR = (1u << gpio->pin);
}

void gpio_set_pull(gpio_t *gpio, gpio_pull_t pull) {
    uint32_t pad = GPIO_PAD(gpio->pin) & ~((1 << 2) | (1 << 3));
    switch (pull) {
        case GPIO_PULL_UP: pad |= (1 << 3); break;
        case GPIO_PULL_DOWN: pad |= (1 << 2); break;
        default: break;
    }
    GPIO_CTRL(gpio->pin) = pad;
    gpio->pull = pull;
}

void gpio_set_func(uint8_t pin, gpio_func_t func) {
    GPIO_CTRL(pin) = (uint32_t)func;
}

gpio_func_t gpio_get_func(uint8_t pin) {
    return (gpio_func_t)GPIO_CTRL(pin);
}

void gpio_free(gpio_t *gpio) {
    if (!gpio) return;
    free(gpio);
}
