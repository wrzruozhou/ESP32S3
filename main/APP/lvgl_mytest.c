//
// Created by Administrator on 2024/12/27.
//

#include "lvgl_mytest.h"
#include "XL9555.h"


lv_obj_t *scr;
lv_obj_t *obj1 = NULL;
lv_obj_t *obj2 = NULL;
lv_obj_t *obj3 = NULL;

lv_style_t img_style;
lv_style_t img_style2;
//图片声明
LV_IMG_DECLARE( True_color )
LV_IMG_DECLARE( True_color_with_alpha )
LV_IMG_DECLARE( True_color_chroma_keyed )

//例程入口函数
void lv_img_test_start()
{
    lv_opa_t  img_opa = LV_OPA_COVER;
    lv_obj_t *scr = lv_scr_act();   /*获取当前活跃的屏幕对象*/
    lv_style_init(&img_style);
    lv_style_set_bg_color(&img_style, lv_color_hex(0x590042));
    lv_style_set_text_color(&img_style, lv_color_hex(0xaccb3));
    lv_style_set_img_opa(&img_style,img_opa);

    lv_style_init(&img_style);
    lv_style_set_img_opa(&img_style2,LV_OPA_50);
    lv_style_set_img_recolor(&img_style2, lv_color_hex(0xff0000));

    //创建图片样式

    //创建显示图标字体和文本的图片对象
    lv_obj_t *img1 = lv_img_create( scr );
    lv_img_set_src( img1,LV_SYMBOL_DUMMY"Icon font: "LV_SYMBOL_AUDIO" audio" );//以文本开头，前面必须加LV_SYMBOL_DUMMY
    lv_obj_add_style(img1,&img_style,0);
    lv_obj_align( img1, LV_ALIGN_TOP_MID, 50, 50 );
    // lv_obj_align_to( img1, scr, LV_ALIGN_TOP_MID, 50, 50 );

    //设置显示True color格式的图片对象
    lv_obj_t *img2 = lv_img_create( scr );
    lv_img_set_src( img2, &True_color );
    lv_obj_add_style(img2,&img_style,0);
    lv_obj_add_style(img2,&img_style2,0);
    lv_obj_align_to( img2,img1,LV_ALIGN_OUT_BOTTOM_MID,0,10 );

    //创建显示Ture color with alpha格式的图片对象
    lv_obj_t *img3 = lv_img_create( scr );
    lv_img_set_src( img3, &True_color_with_alpha );
    lv_obj_add_style(img2,&img_style,0);
    lv_obj_align_to( img3, img2, LV_ALIGN_OUT_BOTTOM_MID,0,10 );

    //创建显示True color chroma keyed格式的图片对象
    lv_obj_t *img4 = lv_img_create( scr );
    lv_img_set_src( img4, &True_color_chroma_keyed );
    lv_img_set_angle(img4 ,90 );
    lv_obj_set_size( img4, 150, 50 );
    lv_obj_align_to( img4,img3,LV_ALIGN_OUT_BOTTOM_MID, 0, 10 );

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, img2);
    lv_anim_set_values(&a,90,180);
    lv_anim_set_time(&a,1000);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_playback_time(&a, 500);
    lv_anim_set_playback_delay(&a, 0);
    lv_anim_set_path_cb(&a, lv_anim_path_linear);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_img_set_angle);
    lv_anim_start(&a);

    lv_anim_t a2;
    a2 = a;
    lv_anim_set_var(&a2, img3);
    lv_anim_start(&a2);
}

//案件处理
void key_handler(void)
{
    static lv_opa_t fpx = LV_OPA_COVER;
    uint8_t key = 0;
    key = xl9555_key_scan(0);
    if (key == KEY0_PRES)
    {
        fpx -= 10;
        if ( fpx <= 10)
            fpx = LV_OPA_COVER;
        printf("the fpx value is %d\n", fpx);
        lv_style_set_img_opa(&img_style, fpx);
        lv_obj_report_style_change(&img_style);         //刷新使用了此样式的对象
    }
}


