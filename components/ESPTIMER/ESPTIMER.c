//
// Created by Administrator on 2024/6/21.
//

#include "ESPTIMER.h"

/**
 * @brief   初始化高精度定时器
 * @param   arr:自动重装载值
 * @param   tps:定时器周期，以us位单位
 *
 * @retval  无
 * */
void EspTimer_Init(uint16_t arr,uint64_t tps)
{
     esp_timer_create_args_t create_args;

     esp_timer_handle_t out_handle;
     create_args.arg = NULL;
     create_args.callback = &esp_timer_callback;
     esp_timer_create(&create_args,&out_handle);
     esp_timer_start_periodic(out_handle,tps);
}

/**
 * @brief   高精度定时器回调函数
 * @param   arg:不携带参数
 * @retval  无*/
void esp_timer_callback(void* arg)
 {
    LED_TOGGLE();
 }
