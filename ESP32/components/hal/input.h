/** @file input.h */
#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>
#include <stdint.h>
#include <esp_err.h>

__BEGIN_DECLS

/** initialize board input
 * @return ESP_OK on success; any other value indicates an error
 */
extern esp_err_t input_init(void);

/** button is released */
#define EVENT_BUTTON_RELEASED false
/** button is pressed */
#define EVENT_BUTTON_PRESSED  true
/** calling from outside ISR */
#define NOT_IN_ISR false
/** calling from inside ISR */
#define IN_ISR true
/** add event to input queue */
extern void input_add_event(uint32_t button_id, bool pressed, bool in_isr);

/** retrieve button input
 * @param timeout the timeout in milliseconds; use -1 for infinite wait
 * @return button_id if button is pressed; 0 if timeout is reached
 */
extern uint32_t input_get_event(int timeout);

/** input button state (bitmap of all buttons)
 * If bit is 0, then button is not pressed, if 1, button is pressed.
 */
extern uint32_t input_button_state;

/** callback method to get notifies on events. */
extern void (*input_notify)(void);

__END_DECLS

#endif // INPUT_H
