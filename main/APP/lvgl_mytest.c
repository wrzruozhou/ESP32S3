//
// Created by Administrator on 2024/12/27.
//

#include "lvgl_mytest.h"
#include "XL9555.h"

#define MAX_VALUE 100
#define MIN_VALUE 0

static void slider_event_cb(lv_event_t * e);

//例程入口函数
void lv_line_test_start()
{
    /*Create a slider in the center of the display*/
    lv_obj_t * slider;
    slider = lv_slider_create( lv_scr_act() );
    lv_obj_center(slider);

    lv_slider_set_mode(slider, LV_SLIDER_MODE_RANGE);
    lv_slider_set_range(slider, MIN_VALUE, MAX_VALUE);
    lv_slider_set_value(slider, 70, LV_ANIM_OFF);
    lv_slider_set_left_value(slider, 20, LV_ANIM_OFF);

    lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_refresh_ext_draw_size(slider);
}

//案件处理
void key_handler(void)
{
    uint8_t key = xl9555_key_scan(0);


}


