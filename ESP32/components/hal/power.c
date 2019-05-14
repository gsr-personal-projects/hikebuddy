#include <sdkconfig.h>

#ifdef CONFIG_HIKEBUDDY_POWER_DEBUG
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif // CONFIG_HIKEBUDDY_POWER_DEBUG

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <esp_log.h>
#include <driver/adc.h>
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "pins.h"
#include "power.h"
#include "pca9555.h"

static const char *TAG = "power";

int battery_volt_sense(void) {
#ifdef ADC1_VBAT_SENSE
	int val = adc1_get_raw(ADC1_VBAT_SENSE);
	if (val == -1)
		return -1;

	return (val * 22 * 9 * 32) >> 12; // modify appropriately TODO
#else
	return -1;
#endif // ADC1_VBAT_SENSE
}

bool usb_connected(void) {
#ifdef PIN_USB_DETECT
	return false; //TODO
#else
	return false;
#endif // PIN_USB_DETECT
}

bool battery_charging(void) {
#ifdef PIN_CHG_STAT
	return false; //TODO
#else
	return false;
#endif // PIN_CHG_STAT
}

static int power_sdcard_gps = 0; // bit 0 = gps, bit 1 = sd-card

esp_err_t power_sdcard_enable(void) {
	return ESP_OK; // TODO
}

esp_err_t power_sdcard_disable(void) {
	return ESP_OK; // TODO
}

esp_err_t power_gps_enable(void) {
	return ESP_OK; // TODO
}

esp_err_t power_gps_disable(void) {
	return ESP_OK; // TODO
}

esp_err_t power_display_enable(void) {
    esp_err_t res;
	res = pca9555_set_output_state(PCA_0_PWR_LCD, 1);
	vTaskDelay(50 / portTICK_PERIOD_MS); // wait for power to stabilize
	return res;
}

esp_err_t power_display_disable(void) {
    esp_err_t res;
	res = pca9555_set_output_state(PCA_0_PWR_LCD, 1);
	return res;
}

esp_err_t power_init(void) {
	static bool power_init_done = false;

	if (power_init_done)
		return ESP_OK;

	ESP_LOGD(TAG, "init called");

	esp_err_t res;

	// configure adc width
#if defined(ADC1_VBAT_SENSE)
	res = adc1_config_width(ADC_WIDTH_12Bit);
	assert( res == ESP_OK );
#endif // defined(ADC1_VBAT_SENSE)

	// configure vbat-sense
#ifdef ADC1_VBAT_SENSE
	// When VDD_A is 3.3V:
	// 0dB attenuation (ADC_ATTEN_0db) since we have a voltage divider on the board
	res = adc1_config_channel_atten(ADC1_VBAT_SENSE, ADC_ATTEN_0db);
	assert( res == ESP_OK );
#endif // ADC1_VBAT_SENSE

	power_init_done = true;

	ESP_LOGD(TAG, "init done");

	return ESP_OK;
}
