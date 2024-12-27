#include "lvgl_demo.h"
#include "RGBLCD.h"
#include "benchmark/lv_demo_benchmark.h"
#include "music/lv_demo_music.h"
#include "stress/lv_demo_stress.h"
#include "esp_timer.h"
#include "lvgl.h"


#include "touch.h"
#include "LED.h"
/**
 * @brief       lvgl_demo入口函数
 * @param       无
 * @retval      无
 */
void lvgl_demo(void)
{
    lv_init();              /* 初始化LVGL图形库 */
    lv_port_disp_init();    /* lvgl显示接口初始化,放在lv_init()的后面 */
    lv_port_indev_init();   /* lvgl输入接口初始化,放在lv_init()的后面 */
    led_init();
    /* 为LVGL提供时基单元 */
    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = &increase_lvgl_tick,
        .name = "lvgl_tick"
    };
    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, 1 * 1000));

    /* 官方demo,需要在SDK Configuration中开启对应Demo */
//    lv_demo_music();                                                              /*空间远远不够*/
//     lv_demo_benchmark();
     lv_demo_widgets();
//     lv_demo_stress();
//     lv_demo_keypad_encoder();
/*这个是gui guide生产的代码*/

#if 0
    setup_ui(&guider_ui);
    events_init(&guider_ui);
#endif

    while (1)
    {
        lv_timer_handler();             /* LVGL计时器 */
        vTaskDelay(pdMS_TO_TICKS(10));  /* 延时10毫秒 */
    }
}

/**
 * @brief       初始化并注册显示设备
 * @param       无
 * @retval      无
 */
void lv_port_disp_init(void)
{
    void *buf1 = NULL;
    void *buf2 = NULL;

    /* 初始化显示设备RGBLCD */
    ltdc_init();                /* RGB屏初始化 */
    ltdc_display_dir(1);        /* 设置横屏 */

    /*-----------------------------
     * 创建一个绘图缓冲区
     *----------------------------*/
    /**
     * LVGL 需要一个缓冲区用来绘制小部件
     * 随后，这个缓冲区的内容会通过显示设备的 'flush_cb'(显示设备刷新函数) 复制到显示设备上
     * 这个缓冲区的大小需要大于显示设备一行的大小
     *
     * 这里有3种缓冲配置:
     * 1. 单缓冲区:
     *      LVGL 会将显示设备的内容绘制到这里，并将他写入显示设备。
     *
     * 2. 双缓冲区:
     *      LVGL 会将显示设备的内容绘制到其中一个缓冲区，并将他写入显示设备。
     *      需要使用 DMA 将要显示在显示设备的内容写入缓冲区。
     *      当数据从第一个缓冲区发送时，它将使 LVGL 能够将屏幕的下一部分绘制到另一个缓冲区。
     *      这样使得渲染和刷新可以并行执行。
     *
     * 3. 全尺寸双缓冲区
     *      设置两个屏幕大小的全尺寸缓冲区，并且设置 disp_drv.full_refresh = 1。
     *      这样，LVGL将始终以 'flush_cb' 的形式提供整个渲染屏幕，您只需更改帧缓冲区的地址。
     */
    /* 使用双缓冲 */
    buf1 = heap_caps_malloc(ltdcdev.width * 60 * sizeof(lv_color_t), MALLOC_CAP_DMA);
    buf2 = heap_caps_malloc(ltdcdev.width * 60 * sizeof(lv_color_t), MALLOC_CAP_DMA);

    /* 初始化显示缓冲区 */
    static lv_disp_draw_buf_t disp_buf;                                 /* 保存显示缓冲区信息的结构体 */
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, ltdcdev.width * 60);   /* 初始化显示缓冲区 */

    /* 在LVGL中注册显示设备 */
    static lv_disp_drv_t disp_drv;      /* 显示设备的描述符(HAL要注册的显示驱动程序、与显示交互并处理与图形相关的结构体、回调函数) */
    lv_disp_drv_init(&disp_drv);        /* 初始化显示设备 */

    /* 设置显示设备的分辨率
     * 这里为了适配正点原子的多款屏幕，采用了动态获取的方式，
     * 在实际项目中，通常所使用的屏幕大小是固定的，因此可以直接设置为屏幕的大小
     */
    disp_drv.hor_res = ltdcdev.width;
    disp_drv.ver_res = ltdcdev.height;

    /* 用来将缓冲区的内容复制到显示设备 */
    disp_drv.flush_cb = lvgl_disp_flush_cb;

    /* 设置显示缓冲区 */
    disp_drv.draw_buf = &disp_buf;

    disp_drv.user_data = panel_handle;

    /* 注册显示设备 */
    lv_disp_drv_register(&disp_drv);
}

/**
 * @brief       初始化并注册输入设备
 * @param       无
 * @retval      无
 */
void lv_port_indev_init(void)
{
    /* 初始化触摸屏 */
    tp_dev.init();

    /* 初始化输入设备 */
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);

    /* 配置输入设备类型 */
    indev_drv.type = LV_INDEV_TYPE_POINTER;

    /* 设置输入设备读取回调函数 */
    indev_drv.read_cb = touchpad_read;

    /* 在LVGL中注册驱动程序，并保存创建的输入设备对象 */
    lv_indev_t *indev_touchpad;
    indev_touchpad = lv_indev_drv_register(&indev_drv);
}

/**
* @brief    将内部缓冲区的内容刷新到显示屏上的特定区域
* @note     可以使用 DMA 或者任何硬件在后台加速执行这个操作
*           但是，需要在刷新完成后调用函数 'lv_disp_flush_ready()'
* @param    disp_drv : 显示设备
* @param    area : 要刷新的区域，包含了填充矩形的对角坐标
* @param    color_map : 颜色数组
* @retval   无
 */
static void lvgl_disp_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
    esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t)drv->user_data;

    /* 特定区域打点 */
    esp_lcd_panel_draw_bitmap(panel_handle, area->x1, area->y1, area->x2 + 1, area->y2 + 1, color_map);

    /* 重要!!! 通知图形库，已经刷新完毕了 */
    lv_disp_flush_ready(drv);
}

/**
 * @brief       告诉LVGL运行时间
 * @param       arg : 传入参数(未用到)
 * @retval      无
 */
static void increase_lvgl_tick(void *arg)
{
    /* 告诉LVGL已经过了多少毫秒 */
    lv_tick_inc(1);
}

/**
 * @brief       获取触摸屏设备的状态
 * @param       无
 * @retval      返回触摸屏设备是否被按下
 */
static bool touchpad_is_pressed(void)
{
    tp_dev.scan(0);     /* 触摸按键扫描 */

    if (tp_dev.sta & TP_PRES_DOWN)
    {
        return true;
    }

    return false;
}


/**
 * @brief       在触摸屏被按下的时候读取 x、y 坐标
 * @param       x   : x坐标的指针
 * @param       y   : y坐标的指针
 * @retval      无
 */
static void touchpad_get_xy(lv_coord_t *x, lv_coord_t *y)
{
    (*x) = tp_dev.x[0];
    (*y) = tp_dev.y[0];
}

/**
 * @brief       图形库的触摸屏读取回调函数
 * @param       indev_drv   : 触摸屏设备
 * @param       data        : 输入设备数据结构体
 * @retval      无
 */
void touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    static lv_coord_t last_x = 0;
    static lv_coord_t last_y = 0;

    /* 保存按下的坐标和状态 */
    if(touchpad_is_pressed())
    {
        touchpad_get_xy(&last_x, &last_y);  /* 在触摸屏被按下的时候读取 x、y 坐标 */
        data->state = LV_INDEV_STATE_PR;
    }
    else
    {
        data->state = LV_INDEV_STATE_REL;
    }

    /* 设置最后按下的坐标 */
    data->point.x = last_x;
    data->point.y = last_y;
}
