//
// Created by Administrator on 2024/6/21.
//

#ifndef __GPTIMER_H
#define __GPTIMER_H

#include "freertos/FreeRTOS.h"
#include <freertos/queue.h>
#include "driver/gpio.h"
#include "driver/gptimer.h"
#include <esp_log.h>

typedef struct {
    uint64_t event_count;
}gptimer_event_t;

extern QueueHandle_t queue;

void gptim_int_init(uint16_t counts,uint16_t reslolution);
bool gptimer_alarm_callback (gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx);

#endif//WRZ_GPTIMER_H
