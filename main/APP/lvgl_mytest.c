//
// Created by Administrator on 2024/12/27.
//

#include "lvgl_mytest.h"
#include "XL9555.h"

const lv_point_t line_points[] = {{10,20},{70,50}, {120,10}, {140,60},{180,10}};    /*坐标点集合*/
#define LINE_POINTS_NUM     (sizeof(line_points)/sizeof(line_points[0]))   /*坐标点的个数*/
lv_obj_t *line1;

//例程入口函数
void lv_line_test_start()
{
    lv_obj_t *scr = lv_scr_act();

    //创建自定义样式
    static lv_style_t line_style;
    lv_style_init(&line_style);
    lv_style_set_line_color(&line_style, lv_color_hex(0xff0000));
    lv_style_set_line_width(&line_style,4);
    lv_style_set_line_rounded(&line_style,1);


    //创建线条对象
    line1 = lv_line_create(scr);
    lv_obj_set_pos(line1,20,20);
    lv_line_set_points(line1,line_points,LINE_POINTS_NUM);
    lv_obj_add_style(line1,&line_style,0);
}

//案件处理
void key_handler(void)
{
    uint8_t key = xl9555_key_scan(0);

    if ( key == KEY0_PRES )
    {
        lv_line_set_y_invert(line1, !lv_line_get_y_invert(line1));
    }
}


