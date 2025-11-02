#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdbool.h>

typedef struct gpio_inst_t gpio_t;
typedef struct uart_inst_t uart_t;

uart_t *uart_init(gpio_t *tx_pin, gpio_t *rx_pin, uint32_t baudrate);

void uart_write_byte(uart_t *uart, uint8_t byte);
uint8_t uart_read_byte(uart_t *uart);

void uart_write(uart_t *uart, const char *str);
int uart_read(uart_t *uart, char *buffer, int max_len);

void uart_free(uart_t *uart);

#endif