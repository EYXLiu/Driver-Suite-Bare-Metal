#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct i2c_inst_t i2c_t;

i2c_t *i2c_config(uint8_t sda, uint8_t scl, uint32_t baudrate);

void i2c_write_byte(i2c_t *i2c, uint8_t addr, uint8_t data);
uint8_t i2c_read_byte(i2c_t *i2c, uint8_t addr);

void i2c_write(i2c_t *i2c, uint8_t addr, uint8_t *data, size_t len);
void i2c_read(i2c_t *i2c, uint8_t addr, uint8_t *data, size_t len);

void i2c_write_reg(i2c_t *i2c, uint8_t addr, uint8_t reg, uint8_t data);
uint8_t i2c_read_reg(i2c_t *i2c, uint8_t addr, uint8_t reg);

void i2c_write_reg_block(i2c_t *i2c, uint8_t addr, uint8_t reg, uint8_t *data, size_t len);
void i2c_read_reg_block(i2c_t *i2c, uint8_t addr, uint8_t reg, uint8_t *data, size_t len);

void i2c_free(i2c_t *i2c);

#endif