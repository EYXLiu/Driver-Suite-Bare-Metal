#ifndef SPI_H
#define SPI_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct gpio_inst_t gpio_t;
typedef struct spi_inst_t spi_t;

spi_t *spi_init(uint8_t sck_pin, uint8_t mosi_pin, uint8_t miso_pin, uint8_t cs_pin, uint32_t baudrate, uint8_t spi_mode);

void spi_select(spi_t *spi);
void spi_deselect(spi_t *spi);

uint8_t spi_transfer_byte(spi_t *spi, uint8_t data);
void spi_transfer(spi_t *spi, uint8_t *tx_data, uint8_t *rx_data, size_t len);

void spi_write(spi_t *spi, uint8_t *data, size_t len);
void spi_read(spi_t *spi, uint8_t *data, size_t len);

void spi_free(spi_t *spi);

#endif