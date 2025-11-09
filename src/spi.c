#include "spi.h"
#include "gpio.h"
#include "irq.h"
#include "addressmap.h"
#include <stdlib.h>
#include <stdint.h>

#define SPI_CR0_OFFSET 0x00
#define SPI_CR1_OFFSET 0x04
#define SPI_DR_OFFSET 0x08
#define SPI_SR_OFFSET 0x0C
#define SPI_CPSR_OFFSET 0x10
#define SPI_IMSC_OFFSET 0x14
#define SPI_MIS_OFFSET 0x18
#define SPI_ICR_OFFSET 0x1C

#define SPI_CR0_DSS_8BIT (0x7 << 0)
#define SPI_CR0_FRF_SPI (0x0 << 4)
#define SPI_CR0_SPO (1 << 6)
#define SPI_CR0_SPH (1 << 7)
#define SPI_CR1_SSE (1 << 1)

#define SPI_SR_TNF (1 << 1)
#define SPI_SR_RNE (1 << 2)
#define SPI_SR_BSY (1 << 4)

#define SPI_CLK 12000000u

#define SPI0 (*(volatile uint32_t *)SPI0_BASE)
#define SPI1 (*(volatile uint32_t *)SPI1_BASE)

#define SPI0_IRQ_NUM 18
#define SPI1_IRQ_NUM 19

#define SPI_IM_RXIM (1 << 2)
#define SPI_IM_TXIM (1 << 3)

struct spi_inst_t {
    volatile uint32_t *base;
    uint8_t sck_pin;
    uint8_t mosi_pin;
    uint8_t miso_pin;
    uint8_t cs_pin;
    uint32_t baudrate;
    uint8_t mode;
    spi_rx_callback_t rx_callback;
    spi_tx_callback_t tx_callback;
};

static spi_t *spi0_ptr;

spi_t *spi_init(uint8_t sck_pin, uint8_t mosi_pin, uint8_t miso_pin, uint8_t cs_pin, uint32_t baudrate, uint8_t spi_mode, spi_rx_callback_t rx_callback, spi_tx_callback_t tx_callback) {
    spi_t *spi = malloc(sizeof(spi_t));
    if (!spi) return NULL;

    spi->base = SPI0;
    spi->sck_pin = sck_pin;
    spi->mosi_pin = mosi_pin;
    spi->miso_pin = miso_pin;
    spi->cs_pin = cs_pin;
    spi->baudrate = baudrate;
    spi->mode = spi_mode;
    spi->rx_callback = rx_callback;
    spi->tx_callback = tx_callback;

    gpio_set_func(sck_pin, GPIO_FUNC_SPI);
    gpio_set_func(mosi_pin, GPIO_FUNC_SPI);
    gpio_set_func(miso_pin, GPIO_FUNC_SPI);

    spi->base[SPI_CR1_OFFSET/4] = 0;

    uint32_t cr0 = SPI_CR0_DSS_8BIT | SPI_CR0_FRF_SPI;
    if (spi_mode & 0x02) cr0 |= SPI_CR0_SPO;
    if (spi_mode & 0x01) cr0 |= SPI_CR0_SPH;
    spi->base[SPI_CR0_OFFSET/4] = cr0;

    uint32_t cpsdvsr = SPI_CLK / (2 * baudrate);
    if (cpsdvsr < 2) cpsdvsr = 2;
    if (cpsdvsr > 254) cpsdvsr = 254;
    spi->base[SPI_CPSR_OFFSET/4]= cpsdvsr & 0xFE;

    spi->base[SPI_CR1_OFFSET/4] = SPI_CR1_SSE;

    spi0_ptr = spi;
    spi->base[SPI_IMSC_OFFSET/4] |= SPI_IM_RXIM;
    irq_enable(SPI0_IRQ_NUM);

    return spi;
}

void spi_select(spi_t *spi) {
    gpio_write_pin(spi->cs_pin, GPIO_LOW);
}

void spi_deselect(spi_t *spi) {
    gpio_write_pin(spi->cs_pin, GPIO_HIGH);
}

uint8_t spi_transfer_byte(spi_t *spi, uint8_t data) {
    volatile uint32_t *base = spi->base;
    
    while (!(base[SPI_SR_OFFSET/4] & SPI_SR_TNF));
    base[SPI_DR_OFFSET/4] = data;

    while (!(base[SPI_SR_OFFSET/4] & SPI_SR_RNE));
    return base[SPI_SR_OFFSET/4];
}

void spi_transfer(spi_t *spi, uint8_t *tx_data, uint8_t *rx_data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        uint8_t tx = tx_data ? tx_data[i] : 0xFF;
        uint8_t rx = spi_transfer_byte(spi, tx);
        if (rx_data) rx_data[i] = rx;
    }
}

void spi_write(spi_t *spi, uint8_t *data, size_t len) {
    spi_transfer(spi, data, NULL, len);
}

void spi_read(spi_t *spi, uint8_t *data, size_t len) {
    spi_transfer(spi, NULL, data, len);
}

void spi_free(spi_t *spi) {
    if (!spi) return;

    irq_disable(SPI0_IRQ_NUM);
    spi->base[SPI_CR1_OFFSET/4] = 0;
    spi0_ptr = NULL;
    
    free(spi);
}

void SPI0_IRQ_Handler(void) {
    if (!spi0_ptr) return;

    uint32_t status = spi0_ptr->base[SPI_MIS_OFFSET/4];
    if ((status & SPI_IM_RXIM) && spi0_ptr->rx_callback) {
        uint8_t data = spi0_ptr->base[SPI_DR_OFFSET/4] & 0xFF;
        spi0_ptr->rx_callback(data);
        spi0_ptr->base[SPI_ICR_OFFSET/4] = SPI_IM_RXIM;
    }
    if ((status & SPI_IM_TXIM) && spi0_ptr->tx_callback) {
        spi0_ptr->tx_callback();
        spi0_ptr->base[SPI_ICR_OFFSET/4] = SPI_IM_TXIM;
    }
}
