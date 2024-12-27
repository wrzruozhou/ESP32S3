//
// Created by Administrator on 2024/6/24.
//

#ifndef __SWPWM_H
#define __SWPWM_H

#include "driver/gpio.h"
#include "driver/ledc.h"

#define LEDC_PWM_TIMER  LEDC_TIMER_2        /*使用定时器0*/
#define LEDC_PWM_CH0_GPIO   GPIO_NUM_1      /*LED控制通道对于GPIO*/
#define LEDC_PWM_CH0_CHANNEL LEDC_CHANNEL_0 /*LED控制通道号*/

void pwm_init(uint8_t resolution,uint16_t fre);
void pwm_setduty(uint16_t duty);
void pwm_hw_init(uint8_t solution, uint16_t fre);
void pwm_hw_setdut(uint16_t duty);

#endif//WRZ_SWPWM_H
