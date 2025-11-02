#include "gpio.h"

void gpio_init(uint8_t pin, gpio_mode_t mode) {
    GPIO_CTRL(pin) = GPIO_FUNC_SIO;

    if (mode == GPIO_MODE_OUTPUT) {
        SIO_GPIO_OE |= (1u << pin);
    } else {
        SIO_GPIO_OE &= ~(1u << pin);
    }
}

void gpio_write(uint8_t pin, gpio_value_t value) {
    if (value == GPIO_LOW) {
        SIO_GPIO_OUT |= (1u << pin);
    } else {
        SIO_GPIO_OUT &= ~(1u << pin);
    }
}

uint8_t gpio_read(uint8_t pin) {
    return (SIO_GPIO_IN & (1u << pin)) != 0;
}

void gpio_toggle(uint8_t pin) {
    SIO_GPIO_OUT_XOR = (1u << pin);
}

void gpio_set_pull(uint8_t pin, gpio_pull_t pull) {
    uint32_t reg = GPIO_CTRL(pin) & ~0x03;
    switch (pull) {
        case GPIO_PULL_NONE: break;
        case GPIO_PULL_UP: reg |= 1; break;
        case GPIO_PULL_DOWN: reg |= 2; break;
    }
    GPIO_CTRL(pin) = reg;
}

void gpio_set_func(uint8_t pin, gpio_func_t func) {
    GPIO_CTRL(pin) = (uint32_t)func;
}
