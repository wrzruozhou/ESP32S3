//
// Created by Administrator on 2024/6/21.
//

#ifndef __ESPTIMER_H
#define __ESPTIMER_H

#include "driver/gpio.h"
#include "esp_timer.h"
#include "LED.h"

void EspTimer_Init(uint16_t arr,uint64_t tps);
void esp_timer_callback(void* arg);

#endif//WRZ_ESPTIMER_H
