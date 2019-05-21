#include <sdkconfig.h>

#ifdef CONFIG_PCA9555_DEBUG
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif // CONFIG_PCA955_DEBUG

#define I2C_PCA9555_ADDR  0x27

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <rom/ets_sys.h>
#include <esp_log.h>
#include <driver/gpio.h>

#include "pins.h"
#include "base.h"
#include "hal_i2c.h"
#include "pca9555.h"
#include "input.h"

#ifdef I2C_PCA9555_ADDR

/*
 Button interrupt masks
 BL     : 95 (0x1000)
 BR     : 9e (0x0100)
 TL     : 97 (0x0800)
 TR     : 9d (0x0200)
 SELECT : 9b (0x0400)
*/
const uint16_t default_input = 0x9f67;
static uint16_t old_input = 0x9f67;
static const char *TAG = "pca9555";

struct pca9555_state_t {
    uint16_t io_direction;         // default is 0x00
    uint16_t output_state;         // default is 0x00
    uint16_t input_state;          // latest read input state
    uint16_t io_write_fail;        // mark bits if write to them failed, so
    uint16_t output_write_fail;    // state is unknown
};

// mutex for accessing pca9555_state, pca9555_handlers, etc..
static xSemaphoreHandle pca9555_mux = NULL;

// semaphore to trigger port-expander interrupt handling
static xSemaphoreHandle pca9555_intr_trigger = NULL;

// port-expander state
static struct pca9555_state_t pca9555_state;

// handlers per port-expander pin
static pca9555_intr_t pca9555_handlers[16] = { NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL };
static void * pca9555_arg[16] = { NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL };

static inline int pca9555_read_reg(uint8_t reg) {
	uint8_t value;
	esp_err_t res = i2c_read_reg(I2C_PCA9555_ADDR, reg, &value, 1);

	if (res != ESP_OK) {
		ESP_LOGE(TAG, "i2c read reg(0x%02x): error %d", reg, res);
		return -1;
	}

/*	ESP_LOGD(TAG, "i2c read reg(0x%02x): 0x%02x", reg, value);*/

	return value;
}

static inline esp_err_t pca9555_write_reg(uint8_t reg, uint8_t value) {
	esp_err_t res = i2c_write_reg(I2C_PCA9555_ADDR, reg, value);

	if (res != ESP_OK) {
		ESP_LOGE(TAG, "i2c write reg(0x%02x, 0x%02x): error %d", reg, value, res);
		return res;
	}

/*	ESP_LOGD(TAG, "i2c write reg(0x%02x, 0x%02x): ok", reg, value);*/

	return ESP_OK;
}

void pca9555_intr_task(void *arg) {
	// we cannot use I2C in the interrupt handler, so we
	// create an extra thread for this..
	// this task reads the interrupt source and updates the input queue

	while (1) {
		if (xSemaphoreTake(pca9555_intr_trigger, portMAX_DELAY)) {
		    uint16_t input = pca9555_get_input();
			uint16_t ints = input^old_input;
			old_input = input;
/*			ESP_LOGD(TAG, "Different inputs on pins: 0x%4x", input);*/
			int i;
			for (i=0; i<16; i++) {
				if (ints & (1 << i)) {
					xSemaphoreTake(pca9555_mux, portMAX_DELAY);
					pca9555_intr_t handler = pca9555_handlers[i];
					void *arg = pca9555_arg[i];
					xSemaphoreGive(pca9555_mux);
/*					ESP_LOGD(TAG, "Button %d %s", i, input & (1 << i) ? "released" : "pressed");*/
					input_add_event(i-7, input & (1 << i) ? false : true, NOT_IN_ISR);
					if (handler != NULL)
						handler(arg);
				}
			}
		}
	}
}

void pca9555_intr_handler(void *arg) {
    /* in interrupt handler */
	int gpio_state = gpio_get_level(PIN_GPIO_INT);
/*	ESP_LOGD(TAG, "gpio_state on INT pin: %d", gpio_state);*/

/*	static int gpio_last_state = -1;*/
/*	if (gpio_state != -1 && gpio_last_state != gpio_state) {*/
/*		ESP_LOGD(TAG, "I2C Int %s", gpio_state == 0 ? "up" : "down");*/
/*	}*/
/*	gpio_last_state = gpio_state;*/

	if (gpio_state == 0) {
		xSemaphoreGiveFromISR(pca9555_intr_trigger, NULL);
	}
}

esp_err_t pca9555_init(void) {
	static bool pca9555_init_done = false;

	if (pca9555_init_done)
		return ESP_OK;

	ESP_LOGD(TAG, "init called");

	esp_err_t res = base_init();
	if (res != ESP_OK)
		return res;

	res = i2c_init();
	if (res != ESP_OK)
		return res;

	pca9555_mux = xSemaphoreCreateMutex();
	if (pca9555_mux == NULL)
		return ESP_ERR_NO_MEM;

	pca9555_intr_trigger = xSemaphoreCreateBinary();
	if (pca9555_intr_trigger == NULL)
		return ESP_ERR_NO_MEM;

	gpio_isr_handler_add(PIN_GPIO_INT, pca9555_intr_handler, NULL);
	gpio_config_t io_conf = {
		.intr_type    = GPIO_INTR_ANYEDGE,
		.mode         = GPIO_MODE_INPUT,
		.pin_bit_mask = 1LL << PIN_GPIO_INT,
		.pull_down_en = 0,
		.pull_up_en   = 1,
	};
	res = gpio_config(&io_conf);
	if (res != ESP_OK)
		return res;

	pca9555_write_reg(0x02, 0x00); // port 0 output values
	pca9555_write_reg(0x03, 0xff); // port 1 output values
	pca9555_write_reg(0x06, 0x00); // port 0, all output
	pca9555_write_reg(0x07, 0xff); // port 1, all input
	struct pca9555_state_t init_state = {
		.io_direction        = 0xff00,
		.output_state        = 0xff00,
		.input_state         = 0xffff,
		.io_write_fail       = 0x0000,
		.output_write_fail   = 0x0000
	};
	memcpy(&pca9555_state, &init_state, sizeof(init_state));

	xTaskCreate(&pca9555_intr_task, "port-expander interrupt task", 2048, NULL, 10, NULL);

	pca9555_intr_handler(NULL);

	pca9555_init_done = true;

	ESP_LOGD(TAG, "init done");

	return ESP_OK;
}

esp_err_t pca9555_set_io_direction(uint8_t pin, uint8_t direction) {
	xSemaphoreTake(pca9555_mux, portMAX_DELAY);

	uint8_t value = pca9555_state.io_direction;
	if (direction)
		value |= 1 << pin;
	else
		value &= ~(1 << pin);

	esp_err_t res = ESP_OK;
	if (pca9555_state.io_direction != value)
	{
		pca9555_state.io_direction = value;
		res = pca9555_write_reg(0x03, value);
		if (res != ESP_OK)
    {
      value = pca9555_state.io_write_fail;
      value |= 1 << pin;
      pca9555_state.io_write_fail = value;
    }
    else
    {
      value = pca9555_state.io_write_fail;
      value &= ~(1 << pin);
      pca9555_state.io_write_fail = value;
    }
	}

	xSemaphoreGive(pca9555_mux);

	return res;
}

esp_err_t pca9555_set_output_state(uint8_t pin, uint8_t state) {
    xSemaphoreTake(pca9555_mux, portMAX_DELAY);

    uint16_t value = pca9555_state.output_state;
    if (state)
	    value |= 1 << pin;
    else
	    value &= ~(1 << pin);

    esp_err_t res = ESP_OK;
    if (pca9555_state.output_state != value) {
	    pca9555_state.output_state = value;
	    res = pca9555_write_reg(0x02, value);
	    if (res != ESP_OK) {
          value = pca9555_state.output_write_fail;
          value |= 1 << pin;
          pca9555_state.output_write_fail = value;
        }
        else {
          value = pca9555_state.output_write_fail;
          value &= ~(1 << pin);
          pca9555_state.output_write_fail = value;
        }
    }

/*    uint16_t buf;*/
/*    i2c_read_reg(0x19, 0x02, &buf, 2);*/
/*    ESP_LOGD(TAG, "Output port states: 0x%04x", buf);*/

    xSemaphoreGive(pca9555_mux);

    return res;
}

void pca9555_set_interrupt_handler(uint8_t pin, pca9555_intr_t handler, void *arg) {
	if (pca9555_mux == NULL) {
	    // allow setting handlers when pca9555 is not initialized yet.
		pca9555_handlers[pin] = handler;
		pca9555_arg[pin] = arg;
	}
	else {
		xSemaphoreTake(pca9555_mux, portMAX_DELAY);

		pca9555_handlers[pin] = handler;
		pca9555_arg[pin] = arg;

		xSemaphoreGive(pca9555_mux);
	}
}

uint16_t pca9555_get_input(void) {
    uint8_t in_0 = pca9555_read_reg(0x00);
    uint8_t in_1 = pca9555_read_reg(0x01);
    uint16_t in = 256U*in_1+in_0;
    pca9555_state.input_state = in;
	return in;
}

#endif // I2C_PCA9555_ADDR
