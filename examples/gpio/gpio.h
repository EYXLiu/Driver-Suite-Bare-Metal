#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum {
    my_GPIO_MODE_INPUT,
    my_GPIO_MODE_OUTPUT
} my_gpio_mode_t;

typedef enum {
    my_GPIO_LOW = 0,
    my_GPIO_HIGH = 1
} my_gpio_value_t;

typedef enum {
    my_GPIO_PULL_NONE,
    my_GPIO_PULL_UP,
    my_GPIO_PULL_DOWN
} my_gpio_pull_t;

typedef enum {
    my_GPIO_FUNC_XPI = 0,
    my_GPIO_FUNC_UART = 1, 
    my_GPIO_FUNC_SPI = 2,
    my_GPIO_FUNC_I2C = 3,
    my_GPIO_FUNC_PWM = 4,
    my_GPIO_FUNC_SIO = 5
} my_gpio_func_t;

#define my_IO_BANK0_BASE ((uintptr_t)0x40014000)
#define my_SIO_BASE 0xD0000000u

#define my_GPIO_CTRL(n) (*(volatile uint32_t*)(my_IO_BANK0_BASE + ((n) * 0x8) + 0x4))

#define my_SIO_GPIO_OUT (*(volatile uint32_t*)(my_SIO_BASE + 0x10))
#define my_SIO_GPIO_OUT_XOR (*(volatile uint32_t*)(my_SIO_BASE + 0x1C))
#define my_SIO_GPIO_IN (*(volatile uint32_t*)(my_SIO_BASE + 0x04))
#define my_SIO_GPIO_OE (*(volatile uint32_t*)(my_SIO_BASE + 0x20))

typedef struct my_gpio_inst_t {
    uint8_t pin;
    my_gpio_mode_t mode;
    my_gpio_pull_t pull;
} my_gpio_t; 

my_gpio_t *my_gpio_init(uint8_t pin, my_gpio_mode_t mode) {
    my_gpio_t *gpio = malloc(sizeof(my_gpio_t));
    if (!gpio) return NULL;

    gpio->pin = pin;
    gpio->mode = mode;
    gpio->pull = my_GPIO_PULL_NONE;

    my_GPIO_CTRL(pin) = my_GPIO_FUNC_SIO;

    if (mode == my_GPIO_MODE_OUTPUT) {
        my_SIO_GPIO_OE |= (1u << pin);
    } else {
        my_SIO_GPIO_OE &= ~(1u << pin);
    }

    return gpio;
}

void my_gpio_write(my_gpio_t *gpio, my_gpio_value_t value) {
    if (value == my_GPIO_LOW) {
        my_SIO_GPIO_OUT &= ~(1u << gpio->pin);
    } else {
        my_SIO_GPIO_OUT |= (1u << gpio->pin);
    }
}

void my_gpio_write_pin(uint8_t pin, my_gpio_value_t value) {
    if (value == my_GPIO_LOW) {
        my_SIO_GPIO_OUT |= (1u << pin);
    } else {
        my_SIO_GPIO_OUT &= ~(1u << pin);
    }
}

uint8_t my_gpio_read(my_gpio_t *gpio) {
    return (my_SIO_GPIO_IN & (1u << gpio->pin)) != 0;
}

uint8_t my_gpio_read_pin(uint8_t pin) {
    return (my_SIO_GPIO_IN & (1u << pin)) != 0;
}

void my_gpio_toggle(my_gpio_t *gpio) {
    my_SIO_GPIO_OUT_XOR = (1u << gpio->pin);
}

void my_gpio_set_pull(my_gpio_t *gpio, my_gpio_pull_t pull) {
    uint32_t reg = my_GPIO_CTRL(gpio->pin) & ~0x03;
    switch (pull) {
        case my_GPIO_PULL_NONE: break;
        case my_GPIO_PULL_UP: reg |= 1; break;
        case my_GPIO_PULL_DOWN: reg |= 2; break;
    }
    my_GPIO_CTRL(gpio->pin) = reg;
    gpio->pull = pull;
}

void my_gpio_set_func(uint8_t pin, my_gpio_func_t func) {
    my_GPIO_CTRL(pin) = (uint32_t)func;
}

my_gpio_func_t my_gpio_get_func(uint8_t pin) {
    return (my_gpio_func_t)my_GPIO_CTRL(pin);
}

void my_gpio_free(my_gpio_t *gpio) {
    if (gpio) free(gpio);
}
