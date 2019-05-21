#include "lv_tutorials.h"
#include <esp_log.h>

void lv_tutorial_hello_world(void)
{
    /*Create a Label on the currently active screen*/
    lv_obj_t * label1 =  lv_label_create(lv_scr_act(), NULL);

    /*Modify the Label's text*/
    lv_label_set_text(label1, "Hello world!");

    /* Align the Label to the center
     * NULL means align on parent (which is the screen now)
     * 0, 0 at the end means an x, y offset after alignment*/
    lv_obj_align(label1, NULL, LV_ALIGN_CENTER, 0, 0);
    ESP_LOGD("Tutorial", "Test");
}

void lv_tutorial_objects(void) {
    
    lv_theme_t *th = lv_theme_mono_init(10, NULL);
    lv_theme_set_current(th);
    
    lv_obj_t * scr = lv_page_create(NULL, NULL);
    lv_scr_load(scr);
    uint16_t scr_h = lv_obj_get_height(scr);
    uint16_t scr_w = lv_obj_get_width(scr);
    ESP_LOGD("lvgl tutorial", "scr size (w x h): %d x %d", scr_w, scr_h);
    
    /****************
     * ADD A TITLE
     ****************/
    lv_obj_t * label = lv_label_create(scr, NULL); /*First parameters (scr) is the parent*/
    lv_label_set_text(label, "Chart and preloader demo");  /*Set the text*/
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
    
    /****************
     * CREATE A CHART
     ****************/
    // set border for chart
    lv_point_t p0 = {0, 0};
    lv_point_t p1 = {scr_w-8, 0};
    lv_point_t p2 = {scr_w-8, scr_h/4};
    lv_point_t p3 = {0, scr_h/4};
    lv_point_t p4 = {0, 0};
    static lv_point_t border[5];
    border[0] = p0;
    border[1] = p1;
    border[2] = p2;
    border[3] = p3;
    border[4] = p4;
    lv_obj_t * line1;
    line1 = lv_line_create(lv_scr_act(), NULL);
    lv_line_set_points(line1, border, 5);     /*Set the points*/
    lv_obj_align(line1, label, LV_ALIGN_OUT_BOTTOM_MID, 3, 0);
    // Now draw the actual chart
    lv_obj_t * chart = lv_chart_create(lv_scr_act(), NULL);  /*Create the chart*/
    lv_obj_set_style(chart, &lv_style_transp);
    lv_obj_set_size(chart, scr_w-8, scr_h / 4);  /*Set the size*/
    lv_obj_align(chart, line1, LV_ALIGN_CENTER, 4, 0);
    lv_chart_set_series_width(chart, 3);  /*Set the line width*/
    /*Add a data series and set some points*/
    lv_chart_series_t * dl1 = lv_chart_add_series(chart, LV_COLOR_BLACK);
    lv_chart_set_next(chart, dl1, 10);
    lv_chart_set_next(chart, dl1, 25);
    lv_chart_set_next(chart, dl1, 45);
    lv_chart_set_next(chart, dl1, 80);
    lv_chart_set_next(chart, dl1, 20);
    lv_chart_set_next(chart, dl1, 30);
    lv_chart_set_next(chart, dl1, 50);
    lv_chart_set_next(chart, dl1, 0);
    
    /****************
     * CREATE A PRELOADER
     ****************/
    
    lv_obj_t * preloader = lv_preload_create(lv_scr_act(), NULL);
    lv_preload_set_arc_length(preloader, 60);
    lv_preload_set_spin_time(preloader, 2000);
    lv_obj_set_size(preloader, scr_w / 4, scr_w / 4);
    lv_obj_align(preloader, line1, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
    /*Create a style for the Preloader*/
/*    static lv_style_t style;*/
/*    lv_style_copy(&style, &lv_style_plain);*/
/*    style.line.width = 4;*/
/*    style.line.color = LV_COLOR_BLACK;*/

/*    style.body.border.color = LV_COLOR_BLACK;*/
/*    style.body.border.width = 2;*/
/*    style.body.padding.hor = 0;*/
/*    lv_preload_set_style(preloader, LV_PRELOAD_STYLE_MAIN, &style);*/
}
