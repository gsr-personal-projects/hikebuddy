#include <sdkconfig.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <rom/ets_sys.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

#include "pins.h"
#include "feedback.h"

static const char *TAG = "feedback";

static int vibrator_on(void) {
  return 0;
}

static int vibrator_off(void) {
  return 0;
}

void vibrator_activate(uint32_t pattern) {
	while (pattern != 0) {
		if ((pattern & 1) == 0)
			vibrator_off();
		else
			vibrator_on();
		pattern >>= 1;
		ets_delay_us(200000);
	}
	vibrator_off();
}

esp_err_t vibrator_init(void) {
	static bool vibrator_init_done = false;
	esp_err_t res;

	if (vibrator_init_done)
		return ESP_OK;

	ESP_LOGD(TAG, "vibrator init called");

	// configure vibrator pin
  // set pin IO options TODO
  // PIN_NUM_VIBRATOR_P pulled down
  // PIN_NUM_VIBRATOR_N pulled down
  // https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/peripherals/gpio.html

	vibrator_init_done = true;

	ESP_LOGD(TAG, "vibrator init done");

	return ESP_OK;
}

static int buzzer_on(void) {
  return 0;
}

static int buzzer_off(void) {
  return 0;
}

void buzzer_activate(uint32_t pattern) {
	while (pattern != 0) {
		if ((pattern & 1) == 0)
			buzzer_off();
		else
			buzzer_on();
		pattern >>= 1;
		ets_delay_us(200000);
	}
	buzzer_off();
}

esp_err_t buzzer_init(void) {
	static bool buzzer_init_done = false;
	esp_err_t res;

	if (buzzer_init_done)
		return ESP_OK;

	ESP_LOGD(TAG, "buzzer init called");

	// configure vibrator pin
  // set pin IO options TODO
  // PIN_NUM_VIBRATOR_P pulled down
  // PIN_NUM_VIBRATOR_N pulled down
  // https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/peripherals/gpio.html

	buzzer_init_done = true;

	ESP_LOGD(TAG, "buzzer init done");

	return ESP_OK;
}
