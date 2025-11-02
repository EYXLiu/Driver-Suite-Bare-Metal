#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdbool.h>

#define UART0_BASE 0x4003400u
#define UART1_BASE 0x4003800u

#define UART0 ((volatile uart_inst_t *)UART0_BASE)
#define UART1 ((volatile uart_inst_t *)UART1_BASE)

#define UART_FR_TXFF (1 << 5)
#define UART_FR_RXFE (1 << 4)

#define UART_LCRH_WLEN_8 (0x3 << 5)
#define UART_CR_UARTEN (1 << 0)
#define UART_CR_TXE (1 << 8)
#define UART_CR_RXE (1 << 8)

#define UART_CLK 12000000u

#define GPIO_FUNC_UART 2

typedef struct {
    uint32_t CR;   // Control register
    uint32_t IBRD; // Integer baud rate divisor
    uint32_t FBRD; // Fractional baud rate divisor
    uint32_t LCRH; // Line control
    uint32_t FR;   // Flag register
    uint32_t DR;   // Data register
    uint32_t IMSC; // Interrupt mask
    uint32_t RIS;  // Raw interrupt status
    uint32_t MIS;  // Masked interrupt status
    uint32_t ICR;  // Interrupt clear
} uart_inst_t;

void uart_init(volatile uart_inst_t *uart, uint32_t tx_pin, uint32_t rx_pin, uint32_t baudrate);

void uart_write_byte(volatile uart_inst_t *uart, uint8_t byte);
uint8_t uart_read_byte(volatile uart_inst_t *uart);

void uart_write(volatile uart_inst_t *uart, const char *str);
int uart_read(volatile uart_inst_t *uart, char *buffer, int max_len);

#endif