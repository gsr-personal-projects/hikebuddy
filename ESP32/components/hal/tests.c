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
#include "bme_test.h"
#include "hal_i2c.h"
#include "pca9555.h"
#include "power.h"
#include "lcd.h"
#include "leds.h"

static const char *TAG = "tests";

void test_lcd_blink(void) {
    for (int i=0; i<10; i++) {
        lcd_paintall(true);
        pca9555_set_output_state(PCA_0_LED_GRN, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        lcd_paintall(false);
        pca9555_set_output_state(PCA_0_LED_GRN, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void test_lcd_scan(void) {
    uint8_t bw = 10;
    uint8_t bh = 10;
    uint16_t bx = 0;
    uint16_t by = 0;
    
    while (1) {
/*        ESP_LOGD(TAG, "x = %d, y = %d", bx, by);*/
        for (uint8_t x=0; x<bw; x++) {
            for (uint8_t y=0; y<bh; y++) {
                lcd_set_pixel(bx+x, by+y, true);
            }
        }
        bx += 5;
        if (bx == WIDTH-bw) {
            bx = 0;
            by += 5;
            if (by == HEIGHT-bh) {by = 0;}
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
        lcd_paintall(false);
    }
}
