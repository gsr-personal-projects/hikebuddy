#include <sdkconfig.h>

#include <stdbool.h>

#include <esp_log.h>
#include <driver/gpio.h>

#include "base.h"

static const char *TAG = "base";

esp_err_t base_init(void) {
	static bool base_init_done = false;

	if (base_init_done)
		return ESP_OK;

	ESP_LOGD(TAG, "init called");

	// install isr-service, so we can register interrupt-handlers per
	// gpio pin.
	esp_err_t res = gpio_install_isr_service(0);
	if (res == ESP_FAIL) {
		ESP_LOGW(TAG, "Failed to install gpio isr service. Ignoring this.");
		res = ESP_OK;
	}

	if (res != ESP_OK) {
		ESP_LOGE(TAG, "Failed to install gpio isr service: %d", res);
		return res;
	}

	base_init_done = true;

	ESP_LOGD(TAG, "init done");

	return ESP_OK;
}
