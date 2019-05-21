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
#include <gpiobutton.h>

#include "hal_i2c.h"
#include "pca9555.h"
#include "power.h"
#include "lcd.h"
#include "leds.h"

#include "lvgl.h"
#include "lv_port.h"

#include "bme280.h"
#include "bme_test.h"
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
    {"BME280", &demo_bme_gui},
    {NULL, NULL},
};

void app_main(void) {
    i2c_init();
    pca9555_init();
    input_init();
    gpiobutton_add(0, 9);
    leds_init();
    power_display_enable();
    lcd_init();
    lcd_enable();
    
    lvgl_init();
    //lv_display_set_orientation(90);
    demo_bme_gui();
    
/*    demo_bme_gui();*/
/*    lcd_disable();*/
/*    power_display_disable();*/
    
    while (1)
    {
        vTaskDelay(10 / portTICK_PERIOD_MS);
/*        leds_init();*/
/*        esp_deep_sleep_start();*/
    }
}

void vPortCleanUpTCB ( void *pxTCB ) {
	// place clean up code here
}
