#ifndef UART_H
#define UART_H

#include "pico/stdlib.h"

void uart_config(uart_inst_t *uart_id, uint tx_pin, uint rx_pin, uint baudrate);

void uart_write_byte(uart_inst_t *uart_id, uint8_t byte);
uint8_t uart_read_byte(uart_inst_t *uart_id);

void uart_write(uart_inst_t *uart_id, const char *str);
void uart_read(uart_inst_t *uart_id, char *buffer, int max_len);

#endif