/**
 * @file lvgl_glue.h
 *
 */

#ifndef LV_PORT_H
#define LV_PORT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lvgl.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
extern void lvgl_disp_flush(int32_t, int32_t, int32_t, int32_t, const lv_color_t*);
extern void lvgl_init();
extern void IRAM_ATTR lv_tick_task();

/**********************
 *      MACROS
 **********************/


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_PORT_H*/
