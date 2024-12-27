//
// Created by Administrator on 2024/12/27.
//

#include "lvgl_mytest.h"

#include <KEY.h>
#include <LED.h>
#include <XL9555.h>
#include <stdio.h>
#include <freertos/task.h>

USER_DATA userData = {
    .name = "feiyangqingyun",
    .age = 20
};

lv_timer_t *timer = NULL;
/*任务回调函数*/
void task1_cb(struct _lv_timer_t * task)
{
    /*获取用户的自定义参数*/
    USER_DATA *dat = (USER_DATA*)task->user_data;
    printf("task1_cb name:%s age:%d\n",dat->name,dat->age);

    /*灯删错提示*/
    LED(1);
    vTaskDelay(1000);
    LED(0);
    vTaskDelay( 1000 );
}

void task_test_start(void )
{
    timer = lv_timer_create(task1_cb, 5000, &userData);
}

/*按键处理函数*/
void key_handled(void)
{
    uint8_t key = xl9555_key_scan( 0 );
    if (timer == NULL)
    {
        return;
    }
    if (key == KEY0_PRES)
    {
        /*使任务复位*/
        lv_timer_reset( timer );
        printf( "0task_reset_tick%ld\r\n", lv_tick_get(  ) );
    }else if (key == KEY1_PRES)
    {
        /*使得任务立刻准备就绪*/
        lv_timer_ready( timer );
        printf( "1task_reset_tick%ld\r\n", lv_tick_get(  ) );
    }else if (key == KEY2_PRES)
    {
        lv_timer_del( timer );
        timer = NULL;
        printf("2task_del_tick:%ld\r\n",lv_tick_get());
    }
}
