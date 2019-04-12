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

#include "bme280.h"

static const char *TAG = "main";

struct menu_item {
  const char *title;
  void (*handler)(void);
};

const struct menu_item demoMenu[] = {
    /*{"UART GPS", &uartdemo},
    {"I2C", &i2cdemo},
    {"uGFX demo", &demoUgfx},
    {"charging demo", &demoPower},*/
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
