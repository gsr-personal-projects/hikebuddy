/**
 * @file lvgl_glue.c
 *
 */

/*
 * WELCOME TO THE LITTLEVGL TUTORIAL
 *
 *-------------------------------------------------------------------------------
 * Now you will learn how to port the Littlev Graphics Library to your system
 * ------------------------------------------------------------------------------
 *
 * 1. Before start download or clone the lvgl folder: https://github.com/littlevgl/lvgl.git
 * 2. Copy 'lvgl' into your project folder
 * 3. Copy 'lvgl/lv_conf_templ.h' and 'lv_conf.h' next to 'lvgl' and remove the first and last #if and  #endif
 * 4. To initialize the library your main.c should look like this file
 *
 *Here are some info about the drawing modes in the library:
 *
 * BUFFERED DRAWING
 * The graphics library works with an internal buffering mechanism to
 * create advances graphics features with only one frame buffer.
 * The internal buffer is called VDB (Virtual Display Buffer) and its size can be adjusted in lv_conf.h.
 * When LV_VDB_SIZE not zero then the internal buffering is used and you have to provide a function
 * which flushes the buffers content to your display.
 *
 * UNBUFFERED DRAWING
 * It is possible to draw directly to a frame buffer when the internal buffering is disabled (LV_VDB_SIZE = 0).
 * Keep in mind this way during refresh some artifacts can be visible because the layers are drawn after each other.
 * And some high level graphics features like anti aliasing, opacity or shadows aren't available in this configuration.
 * In this mode two functions are required:  fill and area with a color AND write a color array to an area
 *
 * HARDWARE ACCELERATION (GPU)
 * If your MCU supports graphical acceleration (GPU) then you can use it with two interface functions:
 * blend two memories using opacity AND fill a memory with a color.
 * Use only one function or none of them is right as well because every GPU function is optional
 */

/*********************
 *      INCLUDES
 *********************/
//#ifdef CONFIG_LVGL_GUI_ENABLE

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include <esp_log.h>

#include "lv_port.h"
#include "lcd.h"

static const char *TAG = "lvgl";

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
//static void IRAM_ATTR lv_tick_task(void);
static void ex_disp_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t * color_p);
static void ex_disp_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2,  lv_color_t color);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

/* Flush the content of the internal buffer the specific area on the display
 * You can use DMA or any hardware acceleration to do this operation in the background but
 * 'lv_flush_ready()' has to be called when finished
 * This function is required only when LV_VDB_SIZE != 0 in lv_conf.h*/
void lvgl_disp_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t * color_p) {
    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/
    ESP_LOGD(TAG, "Inside display_flush");
    int32_t x;
    int32_t y;
    for(y = y1; y <= y2; y++) {
        for(x = x1; x <= x2; x++) {
            /* Put a pixel to the display. For example: */
            lcd_set_pixel((uint8_t)x, (uint8_t)y, (bool)color_p->full);
            color_p++;
        }
    }

    /* IMPORTANT!!!
     * Inform the graphics library that you are ready with the flushing*/
    lv_flush_ready();
}

void lvgl_init() {
    lv_init();
    lv_disp_drv_t disp;
	lv_disp_drv_init(&disp);
	disp.disp_flush = lvgl_disp_flush;
	lv_disp_drv_register(&disp);
	esp_register_freertos_tick_hook(lv_tick_task);
	ESP_LOGD(TAG, "init done");
}

/* Write a pixel array (called 'map') to the a specific area on the display
 * This function is required only when LV_VDB_SIZE == 0 in lv_conf.h*/
static void ex_disp_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t * color_p)
{
    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/
    int32_t x;
    int32_t y;
    for(y = y1; y <= y2; y++) {
        for(x = x1; x <= x2; x++) {
            /* Put a pixel to the display. For example: */
            lcd_set_pixel((uint8_t)x, (uint8_t)y, (bool)color_p->full);
            color_p++;
        }
    }
}


/* Write a pixel array (called 'map') to the a specific area on the display
 * This function is required only when LV_VDB_SIZE == 0 in lv_conf.h*/
static void ex_disp_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2,  lv_color_t color)
{
    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/

    int32_t x;
    int32_t y;
    for(y = y1; y <= y2; y++) {
        for(x = x1; x <= x2; x++) {
            /* Put a pixel to the display. For example: */
            lcd_set_pixel((uint16_t)x, (uint16_t)y, (bool)color.full);
        }
    }

    (void)color; /*Just to avid warnings*/
}

void IRAM_ATTR lv_tick_task(void) {
	lv_tick_inc(portTICK_RATE_MS);
}
//#endif // CONFIG_LVGL_GUI_ENABLE
