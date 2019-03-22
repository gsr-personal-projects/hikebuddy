/** @file leds.h */
#ifndef LEDS_H
#define LEDS_H

#include <stdint.h>
#include <esp_err.h>

__BEGIN_DECLS

/**
 * Initialize the leds driver. (configure SPI bus and GPIO pins)
 * @return ESP_OK on success; any other value indicates an error
 */
extern esp_err_t leds_init(void);

/**
 * Set LED color and brightness
 * @param data the data-bytes to send on the bus.
 * @param len the data-length.
 * @return ESP_OK on success; any other value indicates an error
 */
extern esp_err_t leds_set_color(uint8_t *data, int len);

__END_DECLS

#endif // LEDS_H
