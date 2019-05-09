/** @file board_pins.h */
#ifndef PINS_H
#define PINS_H

#include "sdkconfig.h"

//#ifdef CONFIG_HB_BOARD_V0

#define PCA_0_LED_BLE       0
#define PCA_0_LED_GRN       1
#define PCA_0_LED_RED       2
#define PCA_0_PWR_GPS       3
#define PCA_0_PWR_SDC       4
#define PCA_0_PWR_LCD       6
#define PCA_0_RST_GPS       7
#define PCA_0_EN_LCD        5

#define PCA_1_BTN_LEFT      8
#define PCA_1_BTN_RIGHT     9
#define PCA_1_BTN_UP       10
#define PCA_1_BTN_DOWN     11
#define PCA_1_BTN_SEL      12
#define PCA_1_INT_ACL1     13
#define PCA_1_INT_ACL2     14
#define PCA_1_INT_MAG      15

#define PIN_LCD_CLK        14
#define PIN_LCD_MOSI       13
#define PIN_LCD_CS         15

#define PIN_SDC_CLK        18
#define PIN_SDC_MISO       19
#define PIN_SDC_MOSI       23
#define PIN_SDC_CS          5

#define PIN_GPIO_INT        4
#define PIN_USB_DETECT     27
#define PIN_CHG_STAT       34

#define PIN_I2C_CLK        21
#define PIN_I2C_DATA       22

#define PIN_UART2_RX       32
#define PIN_UART2_TX       33

#define PIN_DAC_VIB         2
#define PIN_DAC_BUZ         1

#define ADC1_VBAT_SENSE     7

#endif // PINS_H
