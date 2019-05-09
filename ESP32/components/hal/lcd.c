#include <sdkconfig.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_err.h>
#include <esp_log.h>
#include <driver/gpio.h>
#include <driver/spi_master.h>

#include "pins.h"
#include "power.h"
#include "pca9555.h"
#include "lcd.h"

#ifndef HIGH
#define HIGH 1
#endif
#ifndef LOW
#define LOW 0
#endif
#define MLCD_WR 0x80
#define MLCD_VCOM 0x40
#define MLCD_CLR 0x20

//TODO define width and height in config menu
#define WIDTH 400
#define HEIGHT 240
#define SPIFREQ 2000000

static const char *TAG = "lcd";
static uint8_t vcom = 0;
static spi_device_handle_t *spi;
static uint8_t *lcd_fb;

/**
The memory LCD uses LSB first transfers (0th data bit is leftmost pixel)
We want half duplex with only the write phase and no read phase
Transaction lengths:
writeline: 8 cmd (1v000000) + 8 line addr + 400 pixels + 16 termination (0's) = 54 bytes (n = 1 in below)
writelines: 8 cmd (1v000000) + (8 addr + 400 pixel + 8 termination) x n_lines + 8 termination = 52n + 2 bytes
changeVCOM: 8 cmd (0v000000) + 8 termination (00000000) = 2 bytes
clrscreen: 8 cmd (0v100000) + 8 termination = 2 bytes

Store address and termination in the buffer? Wastes 480 bytes (3.84 KiB), which is not THAT large among 512 KiB
This saves processing, and hence power

spi_device_interface_config_t:
default length of command and address phase (set cmd to 8 bits)
command bit set to 1 and address bit set to 0 (if address is part of buffer)

READ phase is skipped when SPI_USE_RXDATA is not set

Interrupt or Polling transactions? Interrupt of course!

spi_transaction_t:
cmd and addr fields: cmd and addr bytes to be sent over the bus (only cmd in our case)
SPI_TRANS_VARIABLE_CMD set to 0 to use default length
tx_buffer: buffer to transfer, recommended when DMA is used:
allocate tx_buffer using pvPortMallocCaps(size, MALLOC_CAP_DMA)
tx_buffer should be 32 bit aligned
DMA is not required for payloads < 64 bytes, which is not true if we are always going to send the entire buffer.
Probably better to use DMA and always send the entire buffer (?)
For small payloads, use tx_data to store actual payload, and set the SPI_USE_TXDATA flag; do not touch tx_buffer
**/

esp_err_t lcd_init(void) {
    esp_err_t ret;
    // initialize spi on HSPI device using correct pins
    spi_bus_config_t bus_cfg={
        .mosi_io_num=PIN_LCD_MOSI,
        .sclk_io_num=PIN_LCD_CLK,
        .miso_io_num=-1,
        .quadwp_io_num=-1,
        .quadhd_io_num=-1,
        .max_transfer_sz=(HEIGHT*(WIDTH+16)+16)/8
    };
    ret=spi_bus_initialize(HSPI_HOST, &bus_cfg, 1);
    ESP_ERROR_CHECK(ret);
    //Attach the LCD to the SPI bus
    /*spi_device_interface_config_t dev_cfg={
        .command_bits=8,
        .address_bits=0,
        .dummy_bits=0,
        .mode=0,                                // SPI mode 0 TODO don't know what the 4 numbers mean
        //.cs_ena_pretrans=0,                     // TODO configure to a minimum value that still works with the LCD
        .clock_speed_hz=8000000,//SPIFREQ,
        .spics_io_num=PIN_LCD_CS,
        .queue_size=2,                          // Only one transaction queued at a time TODO
        .flags=SPI_DEVICE_HALFDUPLEX|SPI_DEVICE_BIT_LSBFIRST|SPI_DEVICE_3WIRE,
//        .pre_cb=lcd_spi_pre_transfer_callback,  // Specify pre-transfer callback to handle D/C line TODO
//        .post_cb=                               // Post-transfer callback to get ready for the next frame to be sent
    };*/
    spi_device_interface_config_t dev_cfg={
        .clock_speed_hz=10*1000*1000,           //Clock out at 10 MHz
        .mode=0,                                //SPI mode 0
        .spics_io_num=PIN_LCD_CS,               //CS pin
        .queue_size=7,                          //We want to be able to queue 7 transactions at a time
    };
    ret=spi_bus_add_device(HSPI_HOST, &dev_cfg, spi);
    //ESP_ERROR_CHECK(ret);
    /*// SPI should be initialized now
    // clear the screen
    ret = lcd_clrscr();
    // Flash black then clear again to test
    lcd_paintall(true);
    ret = lcd_invert_vcom();
    vTaskDelay(500 / portTICK_PERIOD_MS);
    lcd_paintall(false);
    ret = lcd_invert_vcom();*/
    return ret;
}

esp_err_t lcd_init_fb(void) {
    lcd_fb = heap_caps_malloc(((HEIGHT*(WIDTH+16))/8+4)*sizeof(uint8_t), MALLOC_CAP_DMA);
    if (lcd_fb != NULL) {
        return ESP_ERR_NO_MEM;
    }
    for (uint8_t i=0; i < HEIGHT; i++) {
        *lcd_fb++ = i;
        lcd_fb += WIDTH/8;
        *lcd_fb++ = (uint8_t)0;
    }
    return ESP_OK;
}

void lcd_paintall(bool black) {
    uint8_t c = 0x00;
    if (black) {c = 0xff;}
    for (uint8_t i=0; i < HEIGHT; i++) {
        *lcd_fb++ = i;
        for (uint8_t j = 0; j < WIDTH/8; j++) {
            *lcd_fb++ = c;
        }
        *lcd_fb++ = (uint8_t)0;
    }
    return;
}

esp_err_t lcd_flush(void) {
    esp_err_t ret;
    static spi_transaction_t fb_trans;
    fb_trans.flags = 0;
    fb_trans.cmd = MLCD_WR;
    fb_trans.length = (HEIGHT*(WIDTH+16)+16)/8;
    fb_trans.tx_buffer = lcd_fb;
    ret=spi_device_transmit(&spi, &fb_trans);
    return ret;
}

esp_err_t lcd_clrscr(void) {
    esp_err_t ret;
    static spi_transaction_t clr_trans;
    clr_trans.flags = SPI_TRANS_USE_TXDATA;
    clr_trans.cmd = MLCD_CLR;
    clr_trans.length = 8;
    clr_trans.tx_data[0] = 0;
    ret=spi_device_transmit(&spi, &clr_trans);
    return ret;
}

esp_err_t lcd_invert_vcom(void) {
    esp_err_t ret;
    static spi_transaction_t vcom_trans;
    vcom_trans.flags = SPI_TRANS_USE_TXDATA;
    if (vcom == 0) vcom = 1;
    else vcom = 0;
    vcom_trans.cmd = MLCD_VCOM*vcom;
    vcom_trans.length = 8;
    vcom_trans.tx_data[0] = 0;
    ret=spi_device_transmit(&spi, &vcom_trans);
    return ret;
}

esp_err_t lcd_enable(void) {
    esp_err_t ret;
	ret = pca9555_set_output_state(PCA_0_EN_LCD, 1);
	return ret;
}

esp_err_t lcd_disable(void) {
    esp_err_t ret;
	ret = pca9555_set_output_state(PCA_0_EN_LCD, 0);
	return ret;
}
