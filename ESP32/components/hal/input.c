#include <sdkconfig.h>

#ifdef CONFIG_HIKEBUDDY_INPUT_DEBUG
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif // CONFIG_HIKEBUDDY_INPUT_DEBUG

#include <esp_event.h>
#include <esp_log.h>

#include "input.h"

static const char *TAG = "input";

xQueueHandle input_queue = NULL;
void (*input_notify)(void);
uint32_t input_button_state = 0;

esp_err_t input_init(void) {
	static bool input_init_done = false;

	if (input_init_done)
		return ESP_OK;

	ESP_LOGD(TAG, "init called");

	input_queue = xQueueCreate(10, sizeof(uint32_t));
	if (input_queue == NULL)
		return ESP_ERR_NO_MEM;
    
	input_init_done = true;

	ESP_LOGD(TAG, "init done");

	return ESP_OK;
}

#ifdef CONFIG_HIKEBUDDY_INPUT_DEBUG
static const char *input_button_name[16] = {
    "(null)",
    "RIGHT",
    "UP",
    "SELECT",
    "LEFT",
    "DOWN",
    "ACCL1",
    "ACCL2",
    "MAGDRDY",
    "FLASH"
};

#endif // CONFIG_HB_INPUT_DEBUG

void input_add_event(uint32_t button_id, bool pressed, bool in_isr) { /* maybe in interrupt handler */
#ifdef CONFIG_HIKEBUDDY_INPUT_DEBUG
/*	ets_printf("input: Button %s %s.\n",*/
/*			input_button_name[button_id < 10 ? button_id : 0],*/
/*			pressed ? "pressed" : "released");*/
#endif // CONFIG_HIKEBUDDY_INPUT_DEBUG

	if (pressed) {
		input_button_state |= 1 << button_id;
		if (in_isr) {
			if (xQueueSendFromISR(input_queue, &button_id, NULL) != pdTRUE) {
				ets_printf("input: input queue full.\n");
			}
		}
		else {
			if (xQueueSend(input_queue, &button_id, 0) != pdTRUE) {
				ets_printf("input: input queue full.\n");
			}
		}
	}
	else {
		input_button_state &= ~(1 << button_id);
	}

	if (input_notify != NULL)
		input_notify();
}

uint32_t input_get_event(int timeout) {
	int xqueuetimeout = (timeout == -1) ? portMAX_DELAY : timeout / portTICK_RATE_MS;
	uint32_t button_id;
	if (xQueueReceive(input_queue, &button_id, xqueuetimeout)) {
		return button_id;
	}
	return 0;
}
