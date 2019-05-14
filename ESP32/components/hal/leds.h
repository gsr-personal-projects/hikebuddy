/** @file leds.h */
#ifndef LEDS_H
#define LEDS_H

#include <stdint.h>
#include <esp_err.h>

__BEGIN_DECLS

typedef struct {
    uint8_t red;
    uint8_t grn;
    uint8_t ble;
} led_color;

/**
 * Initialize the leds driver. (configure SPI bus and GPIO pins)
 * @return ESP_OK on success; any other value indicates an error
 */
extern esp_err_t leds_init(void);

/**
 * Set LED color (in terms of RGB brightness)
 * @param led_color struct with red, grn and ble uint8_t duty cycles
 * @return ESP_OK on success; any other value indicates an error
 */
extern esp_err_t leds_set_color(led_color);

__END_DECLS

#endif // LEDS_H
