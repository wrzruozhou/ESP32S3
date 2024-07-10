//
// Created by Administrator on 2024/6/19.
//

#ifndef __KEY_H
#define __KEY_H

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "LED.h"

#define BOOT gpio_get_level(GPIO_NUM_0)

void key_init(void);
uint8_t key_scan(uint8_t mode);

#endif // WRZ_KEY_H
