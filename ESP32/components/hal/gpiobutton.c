#include <sdkconfig.h>

#include <esp_log.h>
#include <driver/gpio.h>

#include "base.h"
#include "input.h"
#include "gpiobutton.h"

static const char *TAG = "gpiobutton";

uint32_t gpiobutton_conv[40] = { 0 };
int gpiobutton_old_state[40] = { 0 };

void gpiobutton_handler(void *arg) {
    /* in interrupt handler */
	uint32_t gpio_num = (uint32_t) arg;

	int new_state = gpio_get_level(gpio_num);
	if (new_state != gpiobutton_old_state[gpio_num])
	{
		uint32_t button_id = gpiobutton_conv[gpio_num];
		input_add_event(button_id, new_state == 0 ? EVENT_BUTTON_PRESSED : EVENT_BUTTON_RELEASED, IN_ISR);
	}
	gpiobutton_old_state[gpio_num] = new_state;
}

esp_err_t gpiobutton_add(int gpio_num, uint32_t button_id) {
	esp_err_t res = base_init();
	if (res != ESP_OK)
		return res;

	ESP_LOGD(TAG, "add button called");

	gpiobutton_conv[gpio_num] = button_id;
	gpiobutton_old_state[gpio_num] = 1; // released

	res = gpio_isr_handler_add(gpio_num, gpiobutton_handler, (void*) gpio_num);
	if (res != ESP_OK)
		return res;

	// configure the gpio pin for input
	gpio_config_t io_conf = {
		.intr_type    = GPIO_INTR_ANYEDGE,
		.mode         = GPIO_MODE_INPUT,
		.pin_bit_mask = 1LL << gpio_num,
		.pull_down_en = 0,
		.pull_up_en   = 1,
	};
	res = gpio_config(&io_conf);
	if (res != ESP_OK)
		return res;

	ESP_LOGD(TAG, "add button done");

	return res;
}
