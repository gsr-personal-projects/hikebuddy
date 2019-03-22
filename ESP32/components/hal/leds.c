#include <sdkconfig.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_err.h>
#include <esp_log.h>
#include <driver/gpio.h>

#include "pins.h"
#include "power.h"

static const char *TAG = "board_leds";

uint8_t *leds_buf = NULL;
int leds_buf_len = 0;

esp_err_t leds_set_color(uint8_t *data, int len) {
	// TODO display appropriate color on the single LED we have on borad
  return ESP_OK;
}

esp_err_t leds_init(void) {
  // TODO set pin modes for led pins
	return ESP_OK;
}
