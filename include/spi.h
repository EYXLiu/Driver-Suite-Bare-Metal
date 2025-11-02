#ifndef SPI_H
#define SPI_H

#include "pico/stdlib.h"

void spi_config(spi_inst_t *spi_id, uint sck, uint mosi, uint miso, uint cs, uint baudrate, uint8_t spi_mode);

void spi_select(uint cs);
void spi_deselect(uint cs);

uint8_t spi_transfer_byte(spi_inst_t *spi_id, uint8_t data);

void spi_write(spi_inst_t *spi_id, uint8_t *data, size_t len);
void spi_read(spi_inst_t *spi_id, uint8_t *data, size_t len);
void spi_transfer(spi_inst_t *spi_id, uint8_t *tx_data, uint8_t *rx_data, size_t len);

#endif