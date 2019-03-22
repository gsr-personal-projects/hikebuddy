#include <sdkconfig.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <esp_log.h>

#include "pins.h"
#include "sdcard.h"

static const char *TAG = "sdcard";

bool sdcard_detected(void) {
  return true; // TODO
}

esp_err_t sdcard_init(void) {
	static bool sdcard_init_done = false;
	esp_err_t res;

	if (sdcard_init_done)
		return ESP_OK;

	ESP_LOGD(TAG, "init called");
  ESP_LOGW(TAG, "SD Card not yet implemented.");

	sdcard_init_done = true;

	ESP_LOGD(TAG, "init done");

	return ESP_OK;
}
