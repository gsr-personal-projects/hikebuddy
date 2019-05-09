/** @file i2c.h */
#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include <esp_err.h>

__BEGIN_DECLS

/** initialize i2c bus
 * @return ESP_OK on success; any other value indicates an error
 */
extern esp_err_t i2c_init(void);

/** read register via i2c bus
 * @return ESP_OK on success; any other value indicates an error
 */
extern esp_err_t i2c_read_reg(uint8_t addr, uint8_t reg, uint8_t *value, size_t value_len);

/** write to register via i2c bus
 * @return ESP_OK on success; any other value indicates an error
 */
extern esp_err_t i2c_write_reg(uint8_t addr, uint8_t reg, uint8_t value);

/** read event via i2c bus
 * @return ESP_OK on success; any other value indicates an error
 */
extern esp_err_t i2c_read_event(uint8_t addr, uint8_t *buf);

/** write multiple registers on the i2c bus
 * @return ESP_OK on success; any other value indicates an error
 */
extern esp_err_t i2c_write_burst(uint8_t addr, uint8_t reg, uint8_t *value, uint16_t value_len);

__END_DECLS

#endif // I2C_H
