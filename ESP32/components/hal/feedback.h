/** @file feedback.h */
#ifndef FEEDBACK_H
#define FEEDBACK_H

#include <stdint.h>
#include <esp_err.h>

__BEGIN_DECLS

/**
 * Initialize vibrator driver. (GPIO ports)
 * @return ESP_OK on success; any other value indicates an error
 */
extern esp_err_t vibrator_init(void);

/**
 * Send bit-pattern to the vibrator.
 * @note Every bit takes approx. 200ms. Lowest bit is used first.
 *
 * Code example:
 *
 *   vibrator_activate(0xd);
 *   // vibrator will be on for 200ms. Then off for 200ms. Then on for 400ms.
 */
extern void vibrator_activate(uint32_t pattern);

/**
 * Initialize buzzer driver. (GPIO ports)
 * @return ESP_OK on success; any other value indicates an error
 */
extern esp_err_t buzzer_init(void);

/**
 * Send bit-pattern to the buzzer.
 * @note Every bit takes approx. 200ms. Lowest bit is used first.
 *
 * Code example:
 *
 *   buzzer_activate(0xd);
 *   // buzzer will be on for 200ms. Then off for 200ms. Then on for 400ms.
 */
extern void buzzer_activate(uint32_t pattern);

__END_DECLS

#endif // FEEDBACK_H
