/** @file base.h */
#ifndef BASE_H
#define BASE_H

#include <esp_err.h>

__BEGIN_DECLS

/**
 * Initialize board base driver.
 * @return ESP_OK on success; any other value indicates an error
 */
extern esp_err_t base_init(void);

__END_DECLS

#endif // BASE_H
