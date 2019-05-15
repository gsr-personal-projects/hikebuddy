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
    
/*    lv_theme_t *th = lv_theme_mono_init(10, NULL);*/
/*    lv_theme_set_current(th);*/
    
    lv_obj_t * scr = lv_page_create(NULL, NULL);
    lv_scr_load(scr);
    
    /****************
     * ADD A TITLE
     ****************/
    lv_obj_t * label = lv_label_create(scr, NULL); /*First parameters (scr) is the parent*/
    lv_label_set_text(label, "Chart and preloader demo");  /*Set the text*/
    lv_obj_set_x(label, 50);                        /*Set the x coordinate*/
    
    /****************
     * CREATE A CHART
     ****************/
    lv_obj_t * chart = lv_chart_create(lv_scr_act(), NULL);  /*Create the chart*/
    lv_obj_set_size(chart, lv_obj_get_width(scr) / 2, lv_obj_get_width(scr) / 4);  /*Set the size*/
    lv_obj_align(chart, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 0, 50);  /*Align below the slider*/
    lv_chart_set_series_width(chart, 3);  /*Set the line width*/
    /*Add a RED data series and set some points*/
    lv_chart_series_t * dl1 = lv_chart_add_series(chart, LV_COLOR_BLACK);
    lv_chart_set_next(chart, dl1, 10);
    lv_chart_set_next(chart, dl1, 25);
    lv_chart_set_next(chart, dl1, 45);
    lv_chart_set_next(chart, dl1, 80);
    
    /****************
     * CREATE A PRELOADER
     ****************/
    
    lv_obj_t * preloader = lv_preload_create(lv_scr_act(), NULL);
    lv_preload_set_arc_length(preloader, 60);
    lv_preload_set_spin_time(preloader, 2000);
    lv_obj_set_size(preloader, lv_obj_get_width(scr) / 8, lv_obj_get_width(scr) / 8);
    lv_obj_align(preloader, NULL, LV_ALIGN_IN_TOP_RIGHT, -10, 0);
    /*Create a style for the Preloader*/
    static lv_style_t style;
    lv_style_copy(&style, &lv_style_plain);
    style.line.width = 4;
    style.line.color = LV_COLOR_BLACK;

    style.body.border.color = LV_COLOR_BLACK;
    style.body.border.width = 2;
    style.body.padding.hor = 0;
    lv_preload_set_style(preloader, LV_PRELOAD_STYLE_MAIN, &style);
}
