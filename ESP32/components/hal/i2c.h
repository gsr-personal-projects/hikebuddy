/** @file i2c.h */
#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include <esp_err.h>

__BEGIN_DECLS

extern esp_err_t i2c_init(void);

extern esp_err_t i2c_write_bit(uint8_t addr, uint8_t reg, uint8_t bitNum, uint8_t data);
extern esp_err_t i2c_write_bits(uint8_t addr, uint8_t reg, uint8_t bitStart, uint8_t length, uint8_t data);
extern esp_err_t i2c_write_byte(uint8_t addr, uint8_t reg, uint8_t data);
extern esp_err_t i2c_write_bytes(uint8_t addr, uint8_t reg, uint8_t *data, size_t length);

extern esp_err_t i2c_read_bit(uint8_t addr, uint8_t reg, uint8_t bitNum, uint8_t *data);
extern esp_err_t i2c_read_bits(uint8_t addr, uint8_t reg, uint8_t bitStart, uint8_t length, uint8_t *data);
extern esp_err_t i2c_read_byte(uint8_t addr, uint8_t reg, uint8_t *data);
extern esp_err_t i2c_read_bytes(uint8_t addr, uint8_t reg, uint8_t *data, size_t length);

extern esp_err_t i2c_test_connection(uint8_t addr);
extern void i2c_scan(void);

__END_DECLS

#endif // I2C_H
