#include "uart.h"

extern void gpio_set_func(uint8_t pin, uint8_t func);

void uart_init(volatile uart_inst_t *uart, uint32_t tx_pin, uint32_t rx_pin, uint32_t baudrate) {
    gpio_set_func(tx_pin, GPIO_FUNC_UART);
    gpio_set_func(rx_pin, GPIO_FUNC_UART);

    uart->CR = 0;

    float br_div = (float)UART_CLK / (16.0f * baudrate);
    uint32_t int_part = (uint32_t)br_div;
    uint32_t frac_part = (uint32_t)((br_div - int_part) * 64 + 0.5f);

    uart->IBRD = int_part;
    uart->FBRD = frac_part;

    uart->LCRH = UART_LCRH_WLEN_8;
    uart->CR = UART_CR_UARTEN | UART_CR_TXE | UART_CR_RXE;
}

void uart_write_byte(volatile uart_inst_t *uart, uint8_t byte) {
    while (uart->FR & UART_FR_TXFF);
    uart->DR = byte;
}

uint8_t uart_read_byte(volatile uart_inst_t *uart) {
    while (uart->FR & UART_FR_RXFE);
    return (uint8_t)(uart->DR & 0xFF);
}

void uart_write(volatile uart_inst_t *uart, const char *str) {
    while (*str) {
        uart_write_byte(uart, (uint8_t)*str++);
    }
}

int uart_read(volatile uart_inst_t *uart, char *buffer, int max_len) {
    int count = 0;
    while (count < max_len) {
        uint8_t c = uart_read_byte(uart);
        buffer[count++] = c;
        if (c == '\n') break;
    }
    return count;
}
