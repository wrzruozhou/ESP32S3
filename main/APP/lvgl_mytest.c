//
// Created by Administrator on 2024/12/27.
//

#include "lvgl_mytest.h"
#include "XL9555.h"

lv_obj_t *scr;
lv_obj_t *obj1 = NULL;
lv_obj_t *obj2 = NULL;
lv_obj_t *obj3 = NULL;
lv_style_t red_style;

//例程入口函数
void lv_obj_test_start(void)
{
    scr = lv_scr_act(); /*获取当前活跃的屏幕对象*/

    //创建一个基本对象
    obj1 = lv_obj_create( scr );
    /*设置坐标位置*/
    lv_obj_set_pos( obj1, 400, 20 );     /*设置坐标位置*/
    lv_obj_set_size( obj1, 350, 200 );  /*设置大小*/

    //创建一个基本对象2,与对象1进行外部底下居中对齐,同时y轴向上偏移10个像素,
    //目的是为了让obj2有一部分压在obj1上,方便后面演示z轴层级改变的API接口
    obj2 = lv_obj_create( scr );
    lv_obj_set_size( obj2, 200, 200 );
    lv_obj_set_style_bg_color(obj2, lv_color_hex( 0x336699 ),0);
    lv_obj_align_to( obj2, obj1,LV_ALIGN_OUT_BOTTOM_MID, 0, 50 );
}

//案件处理
void key_handler(void)
{
    uint8_t key = xl9555_key_scan( 0 );
    if (key == KEY0_PRES)
    {
        //Z轴层级改变延时,
#define Z_LAYER_MODE    1//1,2,3分别对应3种实现方式
        #if (Z_LAYER_MODE == 1)
            if (obj1)
                lv_obj_move_foreground( obj1 );
#elif (Z_LAYER_MODE == 2)
        lv_obj_move_background( obj2 );
#endif
        printf( "obj1 is on top layer!\r\n" );
    }else if (key == KEY1_PRES)
    {
        //动态创建一个对象3,与屏幕居中对齐,然后更改对象2的父亲为对象3
        obj3 = lv_obj_create( obj1 );
        lv_obj_align_to( obj3,NULL,LV_ALIGN_CENTER,0,0 );
        lv_obj_set_parent( obj2, obj3 );
        lv_obj_set_pos( obj2, 10, 10 );
    }else if (key == KEY2_PRES)
    {
        if (obj1)
        {
            //删除obj1对象,有两种实现方式
#define DEL_MODE 1
#if (DEL_MODE == 1)
            lv_obj_del( obj1 );
#elif (DEL_MODE == 2)
            lv_obj_del_async( obj1 );
#endif
            obj1 = NULL;
            printf( "obj1 is deleted\r\n" );
        }
    }
}


