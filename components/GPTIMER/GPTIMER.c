//
// Created by Administrator on 2024/6/21.
//

#include "GPTIMER.h"


QueueHandle_t queue;

void gptim_int_init(uint16_t counts,uint16_t reslolution)
{
    uint64_t count;

    gptimer_event_callbacks_t cbs;
    gptimer_config_t config;
    gptimer_alarm_config_t  alarm_config;
    gptimer_handle_t  ret_timer = NULL;

    config.clk_src = GPTIMER_CLK_SRC_APB;       /*选择时钟源*/
    config.direction = GPTIMER_COUNT_UP;        /*向上计数*/
    config.resolution_hz = reslolution;         /*计数器的分辨率*/
    cbs.on_alarm = gptimer_alarm_callback;
    alarm_config.alarm_count = 1000000;


    ESP_LOGI("GPTIMER_ALARM","配置通用定时器");

    ESP_ERROR_CHECK(gptimer_new_timer(&config,&ret_timer));

    queue = xQueueCreate(10,sizeof (gptimer_event_t));

    if (queue){
        ESP_LOGE("GPTIMER_ALARM","创建队列成功\n");
    } else{
        ESP_LOGE("GPTIMER_ALARM","创建队列成功\n");
        return;
    }

    /*设置和获取计数值*/
    ESP_LOGI("GPTIMER_ALARM","设置计数值");
    ESP_ERROR_CHECK(gptimer_set_raw_count(ret_timer,counts));
    ESP_LOGI("GPTIMER_ALARM","获取计数值");
    ESP_ERROR_CHECK(gptimer_get_raw_count(ret_timer,&count));
    ESP_LOGI("GPTIMER_ALARM","定时器计数值为：%llu",count);

    /*注册事件回调函数*/
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(ret_timer, &cbs,queue));

    /*设置报警动作*/
    ESP_LOGI("GPTIMER_ALRM","使能通用定时器");
    ESP_ERROR_CHECK(gptimer_enable(ret_timer));
    ESP_ERROR_CHECK(gptimer_set_alarm_action(ret_timer,&alarm_config));
    ESP_ERROR_CHECK(gptimer_start(ret_timer));
}

/**
 * @brief   定时器回调函数
 *
 *
 * @param   无
 * @retval  无*/
bool IRAM_ATTR gptimer_alarm_callback (gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    gptimer_alarm_config_t  config;
    BaseType_t high_task_awoken = pdFALSE;
    queue = (QueueHandle_t)user_ctx;

    /*从时间数据中检索数据*/
    gptimer_event_t ele = {
            .event_count = edata->count_value
    };

    /*可选:通过操作系统队列将事件数据发送到其他任务*/
    xQueueSendFromISR(queue,&ele,&high_task_awoken);

    /*重新配置报警值*/
    config.alarm_count = edata->count_value+1000000;
    gptimer_set_alarm_action(timer,&config);

    /*返回是否需要在ISR退出时是否让步*/
    return high_task_awoken == pdTRUE;
}

/*
 *
uint8_t recod;
gptimer_event_t g_tim_evente;

 uart_init(115200);

 recod = 1;
if (xQueueReceive(queue,&g_tim_evente,2000))
{
    ESP_LOGI("GPTIMER_ALARM","定时器报警，计数值:%llu",g_tim_evente.event_count);
    recod--;
}
else
{
    ESP_LOGW("GPTIMER_ALARM","定时器错过报警");
}*/