#include <sdkconfig.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>
#include <esp_err.h>
#include <esp_log.h>
#include <driver/gpio.h>

#include "pins.h"
#include "pca9555.h"
#include "power.h"
#include "leds.h"

static const char *TAG = "board_leds";
// The buffer used to hold the software timer's data structure.
static StaticTimer_t xPWM_timer_buffer;
static TimerHandle_t pwm_timer;

static led_color sColor;

static void pwm_timer_callback(TimerHandle_t xExpiredTimer) {
    
}

esp_err_t leds_set_color(led_color color) {
    
    return ESP_OK;
}

esp_err_t leds_init(void) {
    pca9555_set_output_state(PCA_0_LED_RED, 1);
    pca9555_set_output_state(PCA_0_LED_BLE, 1);
    pca9555_set_output_state(PCA_0_LED_GRN, 1);
    /*uint8_t pwm_ticks = 0;
    pwm_timer = xTimerCreateStatic("LED PWM Timer",   // Text name for the task.  Helps debugging only.  Not used by FreeRTOS.
                                   1,                 // The period of the timer in ticks.
                                   pdTRUE,            // This is an auto-reload timer.
                                   (void *)pwm_ticks, // A variable incremented by the software timer's callback function
                                   pwm_timer_callback,  // The function to execute when the timer expires.
                                   &xPWM_timer_buffer);
    if(xTimerStart(pwm_timer, 10) != pdPASS ) {
        ESP_LOGE(TAG, "Timer start error");
    }*/
	return ESP_OK;
}
