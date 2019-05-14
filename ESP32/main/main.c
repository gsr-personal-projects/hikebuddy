#include "sdkconfig.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_sleep.h"
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"

#include <esp_log.h>

#include <board.h>
#include <input.h>
#include <pins.h>
#include <button.h>

#include "bme_test.h"
#include "i2c.h"
#include "pca9555.h"
#include "power.h"
#include "lcd.h"
#include "leds.h"

#include "lvgl.h"
#include "lv_port.h"
#include "lv_tutorials.h"

#include "tests.h"

static const char *TAG = "main";

struct menu_item {
    const char *title;
    void (*handler)(void);
};

const struct menu_item level_zero[] = {
    /*{"UART GPS", &demo_gps},
    {"LCD Test", &demo_lvgl},
    {"I2C Peripherals Test", &i2c_scan},*/
    {NULL, NULL},
};

const struct menu_item i2c_devices[] = {
    /*{"Port Expander", &demo_port_expander},
    {"LSM303 Magnetometer", &demo_lsm_mag},
    {"LSM303 Accelerometer", &demo_lsm_accl},*/
    {"BME280", &demo_bme},
    {NULL, NULL},
};

void app_main(void) {
    i2c_init();
    pca9555_init();
    leds_init();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    power_display_enable();
    pca9555_set_output_state(PCA_0_LED_RED, 0);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    lcd_init();
    pca9555_set_output_state(PCA_0_LED_RED, 1);
    pca9555_set_output_state(PCA_0_LED_BLE, 0);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    
    //test_lcd_scan();
    //lvgl_init();lv_init();
    lv_disp_drv_t disp;
	ESP_LOGD(TAG, "initializing lvgl...");
	lv_disp_drv_init(&disp);
	ESP_LOGD(TAG, "registering flush function...");
	disp.disp_flush = lvgl_disp_flush;
	ESP_LOGD(TAG, "registering driver...");
	lv_disp_drv_register(&disp);
	ESP_LOGD(TAG, "registering tick hook...");
	esp_register_freertos_tick_hook(lv_tick_task);
	ESP_LOGD(TAG, "lvgl init done");
    lcd_enable();
    pca9555_set_output_state(PCA_0_LED_BLE, 1);
    lv_tutorial_hello_world();
/*    lcd_disable();*/
/*    power_display_disable();*/
/*    leds_init();*/
    while (1)
    {
/*        esp_deep_sleep_start();*/
    }
}

void vPortCleanUpTCB ( void *pxTCB ) {
	// place clean up code here
}
