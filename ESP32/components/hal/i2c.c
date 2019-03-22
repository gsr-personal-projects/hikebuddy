#include <sdkconfig.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <esp_log.h>
#include <driver/i2c.h>

#include "pins.h"
#include "i2c.h"

#ifdef PIN_I2C_CLK

#define I2C_MASTER_NUM             I2C_NUM_1
#define I2C_MASTER_FREQ_HZ         400000
//define I2C_MASTER_FREQ_HZ        100000
#define I2C_MASTER_TX_BUF_DISABLE  0
#define I2C_MASTER_RX_BUF_DISABLE  0

#define WRITE_BIT      I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT       I2C_MASTER_READ  /*!< I2C master read */
#define ACK_CHECK_EN   0x1     /*!< I2C master will check ack from slave */
#define ACK_CHECK_DIS  0x0     /*!< I2C master will not check ack from slave */
#define ACK_VAL        0x0     /*!< I2C ack value */
#define NACK_VAL       0x1     /*!< I2C nack value */

static const char *TAG = "i2c";

// mutex for accessing the I2C bus
static xSemaphoreHandle i2c_mux = NULL;

esp_err_t i2c_init(void) {
	static bool i2c_init_done = false;

	if (i2c_init_done)
		return ESP_OK;

	ESP_LOGD(TAG, "init called");

	// create mutex for I2C bus
	i2c_mux = xSemaphoreCreateMutex();
	if (i2c_mux == NULL)
		return ESP_ERR_NO_MEM;

	// configure I2C
	i2c_config_t conf = {
		.mode             = I2C_MODE_MASTER,
		.sda_io_num       = PIN_I2C_DATA,
		.sda_pullup_en    = GPIO_PULLUP_ENABLE,
//		.sda_pullup_en    = GPIO_PULLUP_DISABLE,
		.scl_io_num       = PIN_I2C_CLK,
		.scl_pullup_en    = GPIO_PULLUP_ENABLE,
//		.scl_pullup_en    = GPIO_PULLUP_DISABLE,
		.master.clk_speed = I2C_MASTER_FREQ_HZ,
	};
	esp_err_t res = i2c_param_config(I2C_MASTER_NUM, &conf);
	if (res != ESP_OK)
		return res;

	res = i2c_driver_install(I2C_MASTER_NUM, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
	if (res != ESP_OK)
		return res;

	i2c_init_done = true;
  ESP_LOGD(TAG, "I2C initialization successful.\n");
  i2c_scan();

	ESP_LOGD(TAG, "init done");

	return ESP_OK;
}

esp_err_t i2c_write_bit(uint8_t addr, uint8_t reg, uint8_t bitNum, uint8_t data) {
  uint8_t buffer;
  esp_err_t err = i2c_read_byte(addr, reg, &buffer);
  if (err) return err;
  buffer = data ? (buffer | (1 << bitNum)) : (buffer & ~(1 << bitNum));
  return i2c_write_byte(addr, reg, buffer);
}

esp_err_t i2c_write_bits(uint8_t addr, uint8_t reg, uint8_t bitStart, uint8_t length, uint8_t data) {
  uint8_t buffer;
  esp_err_t err = i2c_read_byte(addr, reg, &buffer);
  if (err) return err;
  uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
  data <<= (bitStart - length + 1);
  data &= mask;
  buffer &= ~mask;
  buffer |= data;
  return i2c_write_byte(addr, reg, buffer);
}

esp_err_t i2c_write_byte(uint8_t addr, uint8_t reg, uint8_t data) {
  return i2c_write_bytes(addr, reg, &data, 1);
}

esp_err_t i2c_write_bytes(uint8_t addr, uint8_t reg, uint8_t *data, size_t length) {
  //ets_printf("\nWriting to I2C\n");
	esp_err_t res;
	if (xSemaphoreTake(i2c_mux, portMAX_DELAY) != pdTRUE)
		return ESP_ERR_TIMEOUT;

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	res = i2c_master_start(cmd);
	assert( res == ESP_OK );
	res = i2c_master_write_byte(cmd, ( addr << 1 ) | WRITE_BIT, ACK_CHECK_EN);
	assert( res == ESP_OK );
	res = i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);
	assert( res == ESP_OK );
	res = i2c_master_write(cmd, &data, length, ACK_CHECK_EN);
	assert( res == ESP_OK );
	res = i2c_master_stop(cmd);
	assert( res == ESP_OK );

	res = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);

	if (xSemaphoreGive(i2c_mux) != pdTRUE)
	{
		ESP_LOGE(TAG, "xSemaphoreGive() did not return pdTRUE.");
	}

	return res;
}

esp_err_t i2c_read_bit(uint8_t addr, uint8_t reg, uint8_t bitNum, uint8_t *data) {
  return i2c_read_bits(addr, reg, bitNum, 1, data);
}

esp_err_t i2c_read_bits(uint8_t addr, uint8_t reg, uint8_t bitStart, uint8_t length, uint8_t *data) {
  uint8_t buffer;
  esp_err_t err = i2c_read_byte(addr, reg, &buffer);
  if (!err) {
    uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
    buffer &= mask;
    buffer >>= (bitStart - length + 1);
    *data = buffer;
  }
  return err;
}

esp_err_t i2c_read_byte(uint8_t addr, uint8_t reg, uint8_t *data) {
    return i2c_read_bytes(addr, reg, 1, data);
}

esp_err_t i2c_read_bytes(uint8_t addr, uint8_t reg, uint8_t *data, size_t length) {
  for (int i=0; i<length; i++) {
    data[i] = 0;
  }
	if (xSemaphoreTake(i2c_mux, portMAX_DELAY) != pdTRUE)
		return ESP_ERR_TIMEOUT;

  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (addr << 1) | WRITE_BIT, ACK_CHECK_EN);
  i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (addr << 1) | READ_BIT, ACK_CHECK_EN);
  i2c_master_read(cmd, data, length, I2C_MASTER_LAST_NACK);
  i2c_master_stop(cmd);
  esp_err_t err = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
  i2c_cmd_link_delete(cmd);

	if (xSemaphoreGive(i2c_mux) != pdTRUE) {
		ESP_LOGE(TAG, "xSemaphoreGive() did not return pdTRUE.");
	}

  return err;
}

/*******************************************************************************
 * UTILS
 ******************************************************************************/
esp_err_t i2c_test_connection(uint8_t addr) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (addr << 1) | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    esp_err_t err = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return err;
}

void i2c_scan(void) {
    ets_printf("\n>> I2C scanning for available devices...\n");
    uint8_t count = 0;
    for (size_t i = 0x00; i < 0x7F; i++) {
        if (i2c_test_connection(i) == ESP_OK) {
            ets_printf("- Device found at address 0x%X\n", i);
            count++;
        }
    }
    if (count == 0)
        ets_printf("- No I2C devices found!\n");
    printf("\n");
}

#endif // PIN_I2C_CLK
