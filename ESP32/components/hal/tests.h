/** @file leds.h */
#ifndef TESTS_H
#define TESTS_H

#include <stdint.h>
#include <esp_err.h>
#include "lv_tutorials.h"
#include "bme_test.h"

__BEGIN_DECLS

/**
 * LCD b/w blink test
 */
extern void test_lcd_blink(void);

/**
 * LCD scan pixel box test
 */
extern void test_lcd_scan(void);

__END_DECLS

#endif // TESTS_H
