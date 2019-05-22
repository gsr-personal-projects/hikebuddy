#include <sdkconfig.h>

#ifdef CONFIG_LCD_DEBUG
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif // CONFIG_LCD_DEBUG

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>
#include <esp_err.h>
#include <esp_log.h>
#include <driver/gpio.h>
#include <driver/spi_master.h>

#include "pins.h"
#include "power.h"
#include "pca9555.h"
#include "lcd.h"

static const char *TAG = "lcd";
static uint8_t vcom = 0;
static spi_device_handle_t *spi;
static uint8_t *lcd_fb;
static bool invert = false;
static bool changed = true;

// lcd flush task related declarations
static StaticTask_t xTaskBuffer;
static StackType_t xStack[STACKSIZE]; // TODO optimize this number
static TaskHandle_t xHandle = NULL;

static xSemaphoreHandle fb_access_mux = NULL;

static void lcd_flush_task(void * params) {
    int8_t n = 0;
    while(1) {
        vcom = (vcom == 0) ? 1 : 0;
        if ((n >= VCOMFREQ / FRAMERATE) & changed) {
            n = 0;
            lcd_flush();
        }
        else {
            lcd_update_vcom();
        }
        n += 1;
        vTaskDelay(1000 / portTICK_PERIOD_MS / VCOMFREQ);
    }
}

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
    spi_device_interface_config_t dev_cfg={
        .command_bits=8,
        .address_bits=0,
        .dummy_bits=0,
        .mode=0,
        .cs_ena_pretrans=0,
        .clock_speed_hz=SPIFREQ,
        .spics_io_num=PIN_LCD_CS,
        .queue_size=1,
        .flags=SPI_DEVICE_HALFDUPLEX|SPI_DEVICE_POSITIVE_CS|SPI_DEVICE_BIT_LSBFIRST|SPI_DEVICE_3WIRE,
//        .pre_cb=lcd_spi_pre_transfer_callback,
//        .post_cb=
    };
    ret=spi_bus_add_device(HSPI_HOST, &dev_cfg, &spi);
    ESP_ERROR_CHECK(ret);
    ESP_LOGD(TAG, "SPI added device");
    // SPI should be initialized now
    lcd_init_fb();
    static spi_transaction_t clr_trans;
    clr_trans.flags = SPI_TRANS_USE_TXDATA;
    clr_trans.cmd = MLCD_CLR + vcom*MLCD_VCOM;
    clr_trans.rxlength = 0;
    clr_trans.length = 8;
    clr_trans.tx_data[0] = 0;
    ret = spi_device_queue_trans(spi, &clr_trans, portMAX_DELAY);
	// now start the flush task to update display and flip vcom periodically
    xHandle = xTaskCreateStatic(
                      lcd_flush_task,   /* Function that implements the task. */
                      "LCD FLUSH",      /* Text name for the task. */
                      STACKSIZE,        /* Number of indexes in the xStack array. */
                      (void *) 1,       /* Parameter passed into the task. */
                      0,                /* Priority at which the task is created. */
                      xStack,           /* Array to use as the task's stack. */
                      &xTaskBuffer );   /* Variable to hold the task's data structure. */
    vTaskSuspend(xHandle);
    ESP_LOGD(TAG, "init done");
    return ret;
}

esp_err_t lcd_init_fb(void) {
    fb_access_mux = xSemaphoreCreateMutex();
    lcd_fb = heap_caps_malloc(((HEIGHT*(WIDTH+16))/8+4)*sizeof(uint8_t), MALLOC_CAP_DMA);
    ESP_LOGD(TAG, "Address of lcd_fb = %p", lcd_fb);
    uint8_t *fb_ptr = lcd_fb;
    if (lcd_fb == NULL) {
        return ESP_ERR_NO_MEM;
    }
    lcd_paintall(false);
    ESP_LOGD(TAG, "framebuffer init done");
    return ESP_OK;
}

void lcd_paintall(bool black) {
	if (xSemaphoreTake(fb_access_mux, portMAX_DELAY) != pdTRUE)
		return;// ESP_ERR_TIMEOUT;
    uint8_t c = 0x00;
    if (black) {c = 0xff;}
    uint8_t *fb_ptr = lcd_fb;
    for (uint8_t i=1; i < HEIGHT+1; i++) {
        *fb_ptr++ = i;
        for (uint8_t j = 0; j < WIDTH/8; j++) {
            *fb_ptr++ = c;
        }
        *fb_ptr++ = 0x00;
    }
    *fb_ptr++ = 0x00;
	changed = true;

	if (xSemaphoreGive(fb_access_mux) != pdTRUE) {
		ESP_LOGE(TAG, "xSemaphoreGive() did not return pdTRUE.");
	}
    return;
}

void lcd_set_pixel(uint16_t x, uint16_t y, bool black) {
    if ((x < 0) | (x > WIDTH-1) | (y < 0) | (y > HEIGHT-1)) {return;}
	if (xSemaphoreTake(fb_access_mux, portMAX_DELAY) != pdTRUE)
		return;// ESP_ERR_TIMEOUT;
    // TODO also note the lines that changed, so that only those can be flushed to the display later
/*    ESP_LOGD(TAG, "x, y, color = %d, %d, %d", x, y, black);*/
    uint8_t *fb_ptr = lcd_fb;
/*    ESP_LOGD(TAG, "Original fb_ptr: %p", fb_ptr);*/
    // go to the correct y
    fb_ptr += (2+WIDTH/8)*y;
/*    ESP_LOGD(TAG, "Select line: fb_ptr: %p", fb_ptr);*/
    // now go to correct x
    fb_ptr += 1 + x/8;
/*    ESP_LOGD(TAG, "Select location: fb_ptr: %p", fb_ptr);*/
    uint8_t x_bit = x%8;
    if (invert) {
        if (black) {*fb_ptr &= ~(1<<x_bit);}
        else {*fb_ptr |= 1<<x_bit;}
    }
    else {
        if (black) {*fb_ptr |= 1<<x_bit;}
        else {*fb_ptr &= ~(1<<x_bit);}
    }
	changed = true;

	if (xSemaphoreGive(fb_access_mux) != pdTRUE) {
		ESP_LOGE(TAG, "xSemaphoreGive() did not return pdTRUE.");
	}
    return;
}

esp_err_t lcd_flush(void) {
    esp_err_t ret;
	if (xSemaphoreTake(fb_access_mux, portMAX_DELAY) != pdTRUE)
		return ESP_ERR_TIMEOUT;
    static spi_transaction_t fb_trans;
    fb_trans.flags = 0;
    fb_trans.cmd = MLCD_WR + vcom*MLCD_VCOM; // set or reset the vcom bit
    //ESP_LOGD(TAG, "%x", fb_trans.cmd);
    fb_trans.rxlength = 0;
    fb_trans.length = HEIGHT*(WIDTH+16)+8;
    fb_trans.tx_buffer = lcd_fb;
    // wait for previous transfer to complete (typically this should be complete before the next write anyway)
    spi_transaction_t *rtrans;
    spi_device_get_trans_result(spi, &rtrans, portMAX_DELAY);
    // then send the next frame
    ret = spi_device_queue_trans(spi, &fb_trans, portMAX_DELAY);
/*    ESP_LOGD(TAG, "buffer flushed");*/
    changed = false;

	if (xSemaphoreGive(fb_access_mux) != pdTRUE) {
		ESP_LOGE(TAG, "xSemaphoreGive() did not return pdTRUE.");
	}
    return ret;
}

esp_err_t lcd_clrscr(void) {
    esp_err_t ret;
    static spi_transaction_t clr_trans;
    clr_trans.flags = SPI_TRANS_USE_TXDATA;
    clr_trans.cmd = MLCD_CLR + vcom*MLCD_VCOM;
    clr_trans.rxlength = 0;
    clr_trans.length = 8;
    clr_trans.tx_data[0] = 0;
    // wait for previous transfer to complete (typically this should be complete before the next write anyway)
    spi_transaction_t *rtrans;
    spi_device_get_trans_result(spi, &rtrans, portMAX_DELAY);
    // then send the next frame
    ret = spi_device_queue_trans(spi, &clr_trans, portMAX_DELAY);
    ESP_LOGD(TAG, "screen cleared");
    return ret;
}

esp_err_t lcd_update_vcom(void) {
    esp_err_t ret;
    static spi_transaction_t vcom_trans;
    vcom_trans.flags = SPI_TRANS_USE_TXDATA;
    vcom_trans.cmd =  vcom*MLCD_VCOM;
    //vcom_trans.rxlength = 0;
    vcom_trans.length = 8;
    vcom_trans.tx_data[0] = 0;
    // wait for previous transfer to complete (typically this should be complete before the next write anyway)
    spi_transaction_t *rtrans;
    spi_device_get_trans_result(spi, &rtrans, portMAX_DELAY);
    // then send the next frame
    ret = spi_device_queue_trans(spi, &vcom_trans, portMAX_DELAY);
    //ESP_LOGD(TAG, "VCOM inverted");
    return ret;
}

esp_err_t lcd_enable(void) {
    // TODO first check that the lcd is initialized
    esp_err_t ret;
    // clear the screen (initialize pixel memory), then enable display
    ret = lcd_clrscr();
    vTaskDelay(50 / portTICK_PERIOD_MS); // wait for disp to stabilize
	ret = pca9555_set_output_state(PCA_0_EN_LCD, 1);
    ESP_LOGD(TAG, "enabled display");
    vTaskResume(xHandle);
	vTaskDelay(10 / portTICK_PERIOD_MS); // wait for disp to stabilize
    ESP_LOGD(TAG, "started framebuffer flush task");
	return ret;
}

esp_err_t lcd_disable(void) {
    esp_err_t ret;
    vTaskSuspend(xHandle);
    ESP_LOGD(TAG, "suspended framebuffer flush task");
    // clear the screen (initialize pixel memory), then disable display
    ret = lcd_clrscr();
    vTaskDelay(50 / portTICK_PERIOD_MS); // wait for disp to stabilize
	ret = pca9555_set_output_state(PCA_0_EN_LCD, 0);
    ESP_LOGD(TAG, "device disabled");
	return ret;
}
