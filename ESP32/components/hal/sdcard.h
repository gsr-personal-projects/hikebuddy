/** @file sdcard.h */
#ifndef SDCARD_H
#define SDCARD_H

#include <stdbool.h>
#include <esp_err.h>

__BEGIN_DECLS

/** initialize the sdcard inserted sensor
 * @return ESP_OK on success; any other value indicates an error
 */
extern esp_err_t sdcard_init(void);

/** report if an sdcard is inserted */
extern bool sdcard_detected(void);

__END_DECLS

#endif // SDCARD_H
