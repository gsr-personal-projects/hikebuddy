/** @file pca955.h */
#ifndef PCA9555_H
#define PCA9555_H

#include <esp_err.h>

__BEGIN_DECLS

/** port-expander interrupt handler */
typedef void (*pca9555_intr_t)(void*);

/** initialize port-expander
 * @return ESP_OK on success; any other value indicates an error
 */
extern esp_err_t pca9555_init(void);

/** configure port-expander gpio port - io direction */
extern esp_err_t pca9555_set_io_direction(uint8_t pin, uint8_t direction);
/** configure port-expander gpio port - output state */
extern esp_err_t pca9555_set_output_state(uint8_t pin, uint8_t state);
/** configure port-expander gpio port - set interrupt callback method */
extern void pca9555_set_interrupt_handler(uint8_t pin, pca9555_intr_t handler, void *arg);

/** configure port-expander gpio port - get input status */
extern uint16_t pca9555_get_input(void);

__END_DECLS

#endif // PCA9555_H
