#include <sdkconfig.h>

#include <esp_log.h>
#include <driver/gpio.h>

#include "pins.h"
#include "base.h"
#include "input.h"
#include "gpiobutton.h"
#include "hal_i2c.h"
#include "power.h"
#include "leds.h"
#include "feedback.h"
#include "sdcard.h"
#include "board_nvs.h"

static const char *TAG = "board";

void board_init(void) {
	static bool board_init_done = false;

	if (board_init_done)
		return;

	ESP_LOGD(TAG, "init called");

	// register isr service
	esp_err_t err = base_init();
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "base_init failed: %d", err);
	}

	// initialise nvs config store
	err = board_nvs_init();
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "nvs_init failed: %d", err);
	}

	// configure input queue
	err = input_init();
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "input_init failed: %d", err);
	}

	// configure buttons directly connected to gpio pins
#ifdef PIN_NUM_BTN_1
	err = gpiobutton_add(PIN_NUM_BTN_0, BOARD_BUTTON_LEFT);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "gpiobutton_add failed: %d", err);
	}
	err = gpiobutton_add(PIN_NUM_BTN_1    , BOARD_BUTTON_SELECT);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "gpiobutton_add failed: %d", err);
	}
	err = gpiobutton_add(PIN_NUM_BTN_2, BOARD_BUTTON_RIGHT);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "gpiobutton_add failed: %d", err);
	}
#endif // ! PIN_NUM_BTN_1

	// configure the i2c bus to the port-expander and touch-controller or to the mpr121
#ifdef PIN_NUM_I2C_CLK
	err = i2c_init();
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "i2c_init failed: %d", err);
	}
#endif // PIN_NUM_I2C_CLK

	// configure the voltage measuring for charging-info feedback
	err = power_init();
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "power_init failed: %d", err);
	}

	// configure the led-strip on top of the board
#ifdef PIN_NUM_LEDS_R
	err = leds_init();
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "leds_init failed: %d", err);
	}
#endif // PIN_NUM_LEDS_R

#if defined(PIN_NUM_VIBRATOR_P)
	err = vibrator_init();
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "vibrator_init failed: %d", err);
	}
#endif // defined(PIN_NUM_VIBRATOR_P)

	err = sdcard_init();
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "sdcard_init failed: %d", err);
	}

	board_init_done = true;

	ESP_LOGD(TAG, "init done");
}
