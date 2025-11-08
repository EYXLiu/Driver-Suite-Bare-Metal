#include "uart.h"
#include "gpio.h"
#include <stdlib.h>

#define UART0_BASE 0x40034000u
#define UART1_BASE 0x40038000u

#define UART0 ((volatile uart_inst_t *)UART0_BASE)
#define UART1 ((volatile uart_inst_t *)UART1_BASE)

#define UART_FR_TXFF (1 << 5)
#define UART_FR_RXFE (1 << 4)

#define UART_LCRH_WLEN_8 (0x3 << 5)
#define UART_CR_UARTEN (1 << 0)
#define UART_CR_TXE (1 << 8)
#define UART_CR_RXE (1 << 8)

#define UART_CLK 12000000u

struct uart_inst_t {
    volatile uint32_t *CR;
    volatile uint32_t *IBRD;
    volatile uint32_t *FBRD;
    volatile uint32_t *LCRH;
    volatile uint32_t *FR;
    volatile uint32_t *DR;
    uint8_t tx_pin;
    uint8_t rx_pin;
};

uart_t *uart_init(uint8_t tx_pin, uint8_t rx_pin, uint32_t baudrate) {
    uart_t* uart = malloc(sizeof(uart_t));
    if (!uart) return NULL;

    uart->tx_pin = tx_pin;
    uart->rx_pin = rx_pin;

    gpio_set_func(tx_pin, GPIO_FUNC_UART);
    gpio_set_func(rx_pin, GPIO_FUNC_UART);

    *uart->CR = 0;

    float br_div = (float)UART_CLK / (16.0f * baudrate);
    uint32_t int_part = (uint32_t)br_div;
    uint32_t frac_part = (uint32_t)((br_div - int_part) * 64 + 0.5f);

    *uart->IBRD = int_part;
    *uart->FBRD = frac_part;

    *uart->LCRH = UART_LCRH_WLEN_8;
    *uart->CR = UART_CR_UARTEN | UART_CR_TXE | UART_CR_RXE;

    return uart;
}

void uart_write_byte(uart_t *uart, uint8_t byte) {
    while (*uart->FR & UART_FR_TXFF);
    *uart->DR = byte;
}

uint8_t uart_read_byte(uart_t *uart) {
    while (*uart->FR & UART_FR_RXFE);
    return (uint8_t)(*uart->DR & 0xFF);
}

void uart_write(uart_t *uart, const char *str) {
    while (*str) {
        uart_write_byte(uart, (uint8_t)*str++);
    }
}

int uart_read(uart_t *uart, char *buffer, int max_len) {
    int count = 0;
    while (count < max_len) {
        uint8_t c = uart_read_byte(uart);
        buffer[count++] = c;
        if (c == '\n') break;
    }
    return count;
}

void uart_free(uart_t *uart) {
    if (uart) free(uart);
}
