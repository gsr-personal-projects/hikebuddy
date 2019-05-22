#include <esp_log.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "bme280.h"
#include "bme_test.h"
#include "input.h"

#include "lvgl.h"

LV_FONT_DECLARE(synchronizer_10);

static const char *TAG = "BME Demo";

static char scroll_title[25];
static char scroll_value[15];
static lv_obj_t * scr;
static lv_obj_t * label;
static lv_obj_t * ch_t;
static lv_obj_t * ch_h;
static lv_obj_t * ch_p;
static lv_obj_t * ch_a;
static lv_obj_t * lb_scroll_title;
static lv_obj_t * lb_scroll_value;
static lv_obj_t * lb_scroll_btn_left;
static lv_obj_t * lb_scroll_btn_right;
static lv_chart_series_t * s_t;
static lv_chart_series_t * s_h;
static lv_chart_series_t * s_p;
static lv_chart_series_t * s_a;

typedef enum {
    TEMPERATURE,
    HUMIDITY,
    PRESSURE,
    ALTITUDE
} kind;

typedef struct {
    kind name;
    float value;
} current_data;

static current_data cd[4];
static uint8_t cdptr = 0;

void fill_from_current_data(current_data* cd, char* name, char* value) {
    if (cd->name == TEMPERATURE) {
        snprintf(name, 25, "Temperature");
        snprintf(value, 15, "%.2f C", cd->value);
    }
    if (cd->name == HUMIDITY) {
        snprintf(name, 25, "Humidity");
        snprintf(value, 15, "%.2f %%RH", cd->value);
    }
    if (cd->name == PRESSURE) {
        snprintf(name, 25, "Pressure");
        snprintf(value, 15, "%.2f kPa", cd->value);
    }
    if (cd->name == ALTITUDE) {
        snprintf(name, 25, "Altitude change");
        snprintf(value, 15, "%.2f m", cd->value);
    }
}

void demo_bme_forced(void) {
    bme_init();
    bme_forced_measure();
    set_base_pressure();
    ESP_LOGD(TAG, "Press SELECT key to measure and display readings.");
    ESP_LOGD(TAG, "Press FLASH key to reset altitude.");
    while (1) {
        uint32_t button_id;
        if ((button_id = input_get_event(-1)) != 0) {
            if (button_id == 3) {
                bme_forced_measure();
                read_temperature();
                read_humidity();
                read_altitude();
            }
            if (button_id == 9) {
                bme_forced_measure();
                set_base_pressure();
            }
        }
    }
}

void demo_bme_normal(void) {
    bme_init();
    set_base_pressure();
    while (1) {
        read_temperature();
        read_humidity();
        read_altitude();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void weather_update_task(void) {
    bme_forced_measure();
    cd[0].value = read_temperature();
    cd[2].value = read_pressure() / 1000.0F;
    cd[1].value = read_humidity();
    cd[3].value = read_altitude();
    if (cd[3].value < -100) {cd[3].value = -100;}
    lv_chart_set_next(ch_t, s_t, cd[0].value);
    lv_chart_set_next(ch_h, s_h, cd[1].value);
    lv_chart_set_next(ch_p, s_p, cd[2].value);
    lv_chart_set_next(ch_a, s_a, cd[3].value);
/*    lv_chart_refresh(ch_t);*/
/*    lv_chart_refresh(ch_p);*/
/*    lv_chart_refresh(ch_h);*/
/*    lv_chart_refresh(ch_a);*/
    fill_from_current_data(&cd[cdptr], scroll_title, scroll_value);
    lv_label_set_text(lb_scroll_title, scroll_title);
    lv_label_set_text(lb_scroll_value, scroll_value);
    lv_obj_align(lb_scroll_title, ch_a, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_obj_align(lb_scroll_value, lb_scroll_title, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
}

void button_monitor_task(void) {
    uint32_t button_id;
    while ((button_id = input_get_event(50)) != 0) { // exhaust input queue
        ESP_LOGD(TAG, "Button ID: %d", button_id);
        ESP_LOGD(TAG, "cdptr before: %d", cdptr);
        if (button_id == 4) {
            if (cdptr == 0) {cdptr = 4;}
            cdptr--;
        }
        if (button_id == 1) {
            cdptr++;
            if (cdptr == 4) {cdptr = 0;}
        }
        if (button_id == 9) {
            set_base_pressure();
            weather_update_task();
        }
        ESP_LOGD(TAG, "cdptr after: %d", cdptr);
    }
    fill_from_current_data(&cd[cdptr], scroll_title, scroll_value);
    lv_label_set_text(lb_scroll_title, scroll_title);
    lv_label_set_text(lb_scroll_value, scroll_value);
    lv_obj_align(lb_scroll_title, ch_a, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_obj_align(lb_scroll_value, lb_scroll_title, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
}

void demo_bme_gui(void) {
    bme_init();
    cd[0].name = TEMPERATURE;
    cd[0].value = 0;
    cd[1].name = HUMIDITY;
    cd[1].value = 1;
    cd[2].name = PRESSURE;
    cd[2].value = 2;
    cd[3].name = ALTITUDE;
    cd[3].value = 3;

    lv_theme_t *th = lv_theme_mono_init(10, NULL);
    lv_theme_set_current(th);
    static lv_style_t ch_lb_style;
    lv_style_copy(&ch_lb_style, &lv_style_plain);
    ch_lb_style.line.width = 1;
    ch_lb_style.line.color = LV_COLOR_BLACK;
    ch_lb_style.body.border.width = 0;
    ch_lb_style.body.padding.hor = 0;
    ch_lb_style.body.padding.ver = 0;
    ch_lb_style.body.padding.inner = 0;
    ch_lb_style.text.font = &synchronizer_10;
    static lv_style_t no_pad_style;
    lv_style_copy(&no_pad_style, &lv_style_plain);
    no_pad_style.body.padding.hor = 0;
    no_pad_style.body.padding.ver = 0;
    no_pad_style.body.padding.inner = 0;
    
    scr = lv_page_create(NULL, NULL);
    lv_page_set_style(scr, LV_PAGE_STYLE_BG, &no_pad_style);
    lv_scr_load(scr);
    uint16_t scr_h = lv_obj_get_height(scr);
    uint16_t scr_w = lv_obj_get_width(scr);
    
    label = lv_label_create(scr, NULL);
    lv_label_set_text(label, "Weather monitoring");
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);

    ch_t = lv_chart_create(scr, NULL);
    lv_chart_set_point_count(ch_t, 288);
    lv_obj_set_style(ch_t, &lv_style_transp);
    lv_obj_set_size(ch_t, scr_w-20, scr_h / 5);
    lv_obj_align(ch_t, label, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_chart_set_series_width(ch_t, 3);
    lv_obj_t * lb_t = lv_label_create(ch_t, NULL);
    lv_label_set_text(lb_t, "Temperature");
    lv_obj_align(lb_t, NULL, LV_ALIGN_IN_TOP_LEFT, 5, 8);
    lv_label_set_body_draw(lb_t, true);
    lv_label_set_style(lb_t, &ch_lb_style);
    
    ch_h = lv_chart_create(scr, NULL);
    lv_chart_set_point_count(ch_h, 288);
    lv_obj_set_style(ch_h, &lv_style_transp);
    lv_obj_set_size(ch_h, scr_w-20, scr_h / 5);
    lv_obj_align(ch_h, ch_t, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_chart_set_series_width(ch_h, 3);
    lv_obj_t * lb_h = lv_label_create(ch_h, NULL);
    lv_label_set_text(lb_h, "Humidity");
    lv_obj_align(lb_h, NULL, LV_ALIGN_IN_TOP_LEFT, 5, 8);
    lv_label_set_body_draw(lb_h, true);
    lv_label_set_style(lb_h, &ch_lb_style);
    
    ch_p = lv_chart_create(scr, NULL);
    lv_chart_set_point_count(ch_p, 288);
    lv_obj_set_style(ch_p, &lv_style_transp);
    lv_obj_set_size(ch_p, scr_w-20, scr_h / 5);
    lv_obj_align(ch_p, ch_h, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_chart_set_series_width(ch_p, 3);
    lv_obj_t * lb_p = lv_label_create(ch_p, NULL);
    lv_label_set_text(lb_p, "Pressure");
    lv_obj_align(lb_p, NULL, LV_ALIGN_IN_TOP_LEFT, 5, 8);
    lv_label_set_body_draw(lb_p, true);
    lv_label_set_style(lb_p, &ch_lb_style);
    
    ch_a = lv_chart_create(scr, NULL);
    lv_chart_set_point_count(ch_a, 288);
    lv_obj_set_style(ch_a, &lv_style_transp);
    lv_obj_set_size(ch_a, scr_w-20, scr_h / 5);
    lv_obj_align(ch_a, ch_p, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_chart_set_series_width(ch_a, 3);
    lv_obj_t * lb_a = lv_label_create(ch_a, NULL);
    lv_label_set_text(lb_a, "Altitude");
    lv_obj_align(lb_a, NULL, LV_ALIGN_IN_TOP_LEFT, 5, 8);
    lv_label_set_body_draw(lb_a, true);
    lv_label_set_style(lb_a, &ch_lb_style);
    
    fill_from_current_data(&cd[0], scroll_title, scroll_value);
    
    lb_scroll_title = lv_label_create(scr, NULL);
    lv_obj_align(lb_scroll_title, ch_a, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_label_set_text(lb_scroll_title, "");
    lb_scroll_value = lv_label_create(scr, NULL);
    lv_obj_align(lb_scroll_value, lb_scroll_title, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_label_set_text(lb_scroll_value, "");
    lb_scroll_btn_left = lv_label_create(scr, NULL);
    lv_label_set_text(lb_scroll_btn_left, SYMBOL_LEFT);
    lv_obj_align(lb_scroll_btn_left, ch_a, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 12);
    lb_scroll_btn_right = lv_label_create(scr, NULL);
    lv_label_set_text(lb_scroll_btn_right, SYMBOL_RIGHT);
    lv_obj_align(lb_scroll_btn_right, ch_a, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 12);
    
    s_t = lv_chart_add_series(ch_t, LV_COLOR_BLACK);
    s_p = lv_chart_add_series(ch_p, LV_COLOR_BLACK);
    s_h = lv_chart_add_series(ch_h, LV_COLOR_BLACK);
    s_a = lv_chart_add_series(ch_a, LV_COLOR_BLACK);
    lv_chart_set_div_line_count(ch_t, 1, 5);
    lv_chart_set_div_line_count(ch_h, 1, 5);
    lv_chart_set_div_line_count(ch_p, 1, 5);
    lv_chart_set_div_line_count(ch_a, 1, 5);
    lv_chart_set_range(ch_t, -10, 40);
    lv_chart_set_range(ch_h, 10, 80);
    lv_chart_set_range(ch_p, 90, 110);
    lv_chart_set_range(ch_a, -10, 50);
/*    lv_chart_init_points(ch_t, s_t, 24);*/
/*    lv_chart_init_points(ch_p, s_p, 100);*/
/*    lv_chart_init_points(ch_h, s_h, 50);*/
/*    lv_chart_init_points(ch_a, s_a, 0);*/
    
    lv_task_create(button_monitor_task, 400, LV_TASK_PRIO_MID, NULL);
    lv_task_create(weather_update_task, 300000, LV_TASK_PRIO_MID, NULL);
/*    xTaskCreate(&weather_update_task, "weather update task", 2048, NULL, 1, NULL);*/
}
