/** @file lcd.h */
#ifndef LCD_H
#define LCD_H

#include <stdint.h>
#include <esp_err.h>

__BEGIN_DECLS

/**
 * Initialize the lcd driver. (configure SPI bus and GPIO pins)
 * @return ESP_OK on success; any other value indicates an error
 */
extern esp_err_t lcd_init(void);

/**
 * Initialize the framebuffer
 * @return ESP_OK on success; any other value indicates an error
 */
extern esp_err_t lcd_init_fb(void);

/**
 * Transmit framebuffer to lcd.
 * @return ESP_OK on success; any other value indicates an error
 */
extern esp_err_t lcd_transmit_buffer(void);

/**
 * Clear screen
 * @return ESP_OK on success; any other value indicates an error
 */
extern esp_err_t lcd_clrscr(void);

/**
 * Invert VCOM
 * Call if screen is enabled but hasn't changed in a *long* time
 * @return ESP_OK on success; any other value indicates an error
 */
extern esp_err_t lcd_invert_vcom(void);

/**
 * Fill buffer with all 1 or 0
 * @return ESP_OK on success; any other value indicates an error
 */
extern void lcd_paintall(bool);

/**
 * Flush buffer
 * Sends framebuffer to screen
 * @return ESP_OK on success; any other value indicates an error
 */
extern esp_err_t lcd_flush(void);

/**
 * Enable LCD display (this is different from powering it on or off)
 * When disabled, the memory is still retained, only no longer shown
 * @return ESP_OK on success; any other value indicates an error
 */
extern esp_err_t lcd_enable(void);

/**
 * Disable LCD display (this is different from powering it on or off)
 * When disabled, the memory is still retained, only no longer shown
 * @return ESP_OK on success; any other value indicates an error
 */
extern esp_err_t lcd_disable(void);

__END_DECLS

#endif // LCD_H
