#include <esp_log.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "math.h"

#include "hal_i2c.h"
#include "lsm303.h"

static const char *TAG = "LSM303";
