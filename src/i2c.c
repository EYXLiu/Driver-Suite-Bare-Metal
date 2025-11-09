#include "i2c.h"
#include "gpio.h"
#include "addressmap.h"
#include <stdlib.h>
#include <stdint.h>

#define IC_CON_OFFSET 0x00
#define IC_TAR_OFFSET 0x04
#define IC_DATA_CMD 0x10
#define IC_SS_SCL_HCNT 0x14
#define IC_SS_SCL_LCNT 0x18
#define IC_ENABLE_OFFSET 0x6C
#define IC_STATUS_OFFSET 0x70

#define IC_CON_MASTER_MODE (1 << 0)
#define IC_CON_SPEED_STD (0 << 1)
#define IC_CON_SPEED_FAST (1 << 1)
#define IC_CON_SLAVE_DISABLE (1 << 6)
#define IC_CON_10BITADDR_MASTER (1 << 4)

#define IC_ENABLE_ENABLE (1 << 0)
#define IC_STATUS_TFNF (1 << 1)
#define IC_STATUS_RNE (1 << 3)

#define IC_DATA_CMD_CMD (1 << 8)
#define IC_DATA_CMD_STOP (1 << 9)

struct i2c_inst_t {
    volatile uint32_t *base;
    uint8_t sda_pin;
    uint8_t scl_pin;
    uint32_t baudrate;
};

void i2c_wait_tx_empty(i2c_t *i2c) {
    while (!(i2c->base[IC_STATUS_OFFSET/4] & IC_STATUS_TFNF));
}

void i2c_wait_rx_nonempty(i2c_t *i2c) {
    while (!(i2c->base[IC_STATUS_OFFSET/4] & IC_STATUS_RNE));
}

i2c_t *i2c_config(uint8_t sda, uint8_t scl, uint32_t baudrate) {
    i2c_t *i2c = malloc(sizeof(i2c_t));
    if (!i2c) return NULL;

    i2c->base = (volatile uint32_t *)(I2C0_BASE);
    i2c->sda_pin = sda;
    i2c->scl_pin = scl;
    i2c->baudrate = baudrate;

    gpio_set_func(sda, GPIO_FUNC_I2C);
    gpio_set_func(scl, GPIO_FUNC_I2C);

    i2c->base[IC_ENABLE_OFFSET/4] = 0;

    i2c->base[IC_CON_OFFSET/4] = IC_CON_MASTER_MODE | IC_CON_SLAVE_DISABLE | IC_CON_SPEED_STD;

    i2c->base[IC_TAR_OFFSET/4] = 0; 

    i2c->base[IC_ENABLE_OFFSET/4] = IC_ENABLE_ENABLE;

    return i2c;
}

void i2c_write_byte(i2c_t *i2c, uint8_t addr, uint8_t data) {
    i2c->base[IC_TAR_OFFSET/4] = addr;
    i2c_wait_tx_empty(i2c);
    i2c->base[IC_DATA_CMD/4] = (data & 0xFF) | IC_DATA_CMD_STOP;
}

uint8_t i2c_read_byte(i2c_t *i2c, uint8_t addr) {
    i2c->base[IC_TAR_OFFSET/4] = addr;
    i2c_wait_tx_empty(i2c);
    i2c->base[IC_DATA_CMD/4] = IC_DATA_CMD_CMD;
    i2c_wait_rx_nonempty(i2c);
    return i2c->base[IC_DATA_CMD/4] & 0xFF;
}

void i2c_write(i2c_t *i2c, uint8_t addr, uint8_t *data, size_t len) {
    i2c->base[IC_TAR_OFFSET/4] = addr;
    for (size_t i = 0; i < len; i++) {
        i2c_wait_tx_empty(i2c);
        uint32_t cmd = data[i] & 0xFF;
        if (i == len-1) cmd |= IC_DATA_CMD_STOP;
        i2c->base[IC_DATA_CMD/4] = cmd;
    }
}

void i2c_read(i2c_t *i2c, uint8_t addr, uint8_t *data, size_t len) {
    i2c->base[IC_TAR_OFFSET/4] = addr;
    for (size_t i = 0; i < len; i++) {
        i2c_wait_tx_empty(i2c);
        i2c->base[IC_DATA_CMD/4] = IC_DATA_CMD_CMD;
        i2c_wait_rx_nonempty(i2c);
        data[i] = i2c->base[IC_DATA_CMD/4] & 0xFF;
    }
}

void i2c_write_reg(i2c_t *i2c, uint8_t addr, uint8_t reg, uint8_t data) {
    i2c->base[IC_TAR_OFFSET/4] = addr;
    i2c_wait_tx_empty(i2c);
    i2c->base[IC_DATA_CMD/4] = reg & 0xFF;
    i2c_wait_tx_empty(i2c);
    i2c->base[IC_DATA_CMD/4] = (data & 0xFF) | IC_DATA_CMD_STOP;
}

uint8_t i2c_read_reg(i2c_t *i2c, uint8_t addr, uint8_t reg) {
    i2c->base[IC_TAR_OFFSET/4] = addr;
    i2c_wait_tx_empty(i2c);
    i2c->base[IC_DATA_CMD/4] = reg & 0xFF;
    i2c_wait_tx_empty(i2c);
    i2c->base[IC_DATA_CMD/4] = IC_DATA_CMD_CMD | IC_DATA_CMD_STOP;
    i2c_wait_rx_nonempty(i2c);
    return i2c->base[IC_DATA_CMD/4] & 0xFF;
}

void i2c_write_reg_block(i2c_t *i2c, uint8_t addr, uint8_t reg, uint8_t *data, size_t len) {
    i2c->base[IC_TAR_OFFSET/4] = addr;
    i2c_wait_tx_empty(i2c);
    i2c->base[IC_TAR_OFFSET/4] = reg & 0xFF;
    for (size_t i = 0; i < len; i++) {
        i2c_wait_tx_empty(i2c);
        uint32_t cmd = data[i] & 0xFF;
        if (i == len-1) cmd |= IC_DATA_CMD_STOP;
        i2c->base[IC_TAR_OFFSET/4] = cmd;
    }
}

void i2c_read_reg_block(i2c_t *i2c, uint8_t addr, uint8_t reg, uint8_t *data, size_t len) {
    i2c->base[IC_TAR_OFFSET/4] = addr;
    i2c_wait_tx_empty(i2c);
    i2c->base[IC_DATA_CMD/4] = reg & 0xFF;
    for (size_t i = 0; i < len; i++) {
        i2c_wait_tx_empty(i2c);
        uint32_t cmd = IC_DATA_CMD_CMD;
        if (i == len-1) cmd |= IC_DATA_CMD_STOP;
        i2c->base[IC_DATA_CMD/4] = cmd;
        i2c_wait_rx_nonempty(i2c);
        data[i] = i2c->base[IC_DATA_CMD/4] & 0xFF;
    }
}

void i2c_free(i2c_t *i2c) {
    if (!i2c) return;
    free(i2c);
}
