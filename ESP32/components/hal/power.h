/** @file power.h */
#ifndef POWER_H
#define POWER_H

#include <stdbool.h>
#include <esp_err.h>

__BEGIN_DECLS

/**
 * initializes the battery and usb power sensing
 * @return ESP_OK on success; any other value indicates an error
 */
extern esp_err_t power_init(void);

/**
 * returns the charging state.
 *
 * @return true if charging; false otherwise
 */
extern bool battery_charging(void);

/**
 * returns the Vbat voltage.
 *
 * @return Vbat voltage in mV; -1 on error
 */
extern int battery_volt_sense(void);

/**
 * returns the USB connection state.
 *
 * @return true if USB is connected
 */
extern bool usb_connected(void);

/**
 * enable power to GPS
 *
 * @return ESP_OK on success; any other value indicates an error
 */
extern esp_err_t power_gps_enable(void);

/**
 * disable power to GPS
 *
 * @return ESP_OK on success; any other value indicates an error
 */
extern esp_err_t power_gps_disable(void);

/**
 * enable power to the sd-card
 *
 * @return ESP_OK on success; any other value indicates an error
 */
extern esp_err_t power_sdcard_enable(void);

/**
 * disable power to the sd-card
 *
 * @return ESP_OK on success; any other value indicates an error
 */
extern esp_err_t power_sdcard_disable(void);

/**
 * enable power to the display
 *
 * @return ESP_OK on success; any other value indicates an error
 */
extern esp_err_t power_display_enable(void);

/**
 * disable power to the display
 *
 * @return ESP_OK on success; any other value indicates an error
 */
extern esp_err_t power_display_disable(void);

__END_DECLS

#endif // POWER_H
