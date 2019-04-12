#include "sdkconfig.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include <string.h>

#include <esp_log.h>

#include <board.h>
#include <input.h>
#include <pins.h>
#include <button.h>

#include "bme_test.h"
#include "i2c.h"

static const char *TAG = "main";

struct menu_item {
  const char *title;
  void (*handler)(void);
};

const struct menu_item level_zero[] = {
    /*{"UART GPS", &demo_gps},
    {"LCD Test", &demo_lvgl},*/
    {"I2C Peripherals Test", &i2c_scan},
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
  while (1)
  {
    esp_deep_sleep_start();
  }
}

void vPortCleanUpTCB ( void *pxTCB ) {
	// place clean up code here
}
