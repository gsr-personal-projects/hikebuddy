/** @file gpiobutton.h */
#ifndef GPIOBUTTON_H
#define GPIOBUTTON_H

#include <stdint.h>
#include <esp_err.h>

__BEGIN_DECLS

/** configure gpio pin as button
 * @return ESP_OK on success; any other value indicates an error
 */
extern esp_err_t gpiobutton_add(int gpio_num, uint32_t button_id);

__END_DECLS

#endif // GPIOBUTTON_H
