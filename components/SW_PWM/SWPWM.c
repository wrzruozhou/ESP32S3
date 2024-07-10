//
// Created by Administrator on 2024/6/24.
//

#include "SWPWM.h"

/**
 * @brief   PWM初始化
 * @param   两个
 * @arg     resolution:占空比分辨率
 * @arg     fre:频率
 * @retval  无*/
void pwm_init(uint8_t resolution,uint16_t fre)
{
    ledc_timer_config_t  timer_conf;
    timer_conf.speed_mode = LEDC_LOW_SPEED_MODE;            /*定时器模式:只能用低速模式*/
    timer_conf.clk_cfg = LEDC_USE_APB_CLK;                  /*定时器使用的时钟源*/
    timer_conf.freq_hz = fre;                               /*定时器的频率*/
    timer_conf.timer_num =  LEDC_PWM_TIMER;                 /*选择的定时器*/
    timer_conf.duty_resolution = resolution;                /*定时器的占空比分辨率*/
    ledc_timer_config(&timer_conf);

    /*配置通道*/
    ledc_channel_config_t  ledc_conf;
    ledc_conf.gpio_num = LEDC_PWM_CH0_GPIO;                 /*选中所需的引脚*/
    ledc_conf.intr_type = LEDC_INTR_DISABLE;                /*禁用中断*/
    ledc_conf.channel   = LEDC_PWM_CH0_CHANNEL;             /*选中通道0*/
    ledc_conf.duty = 0;                                     /*占空比值0*/
    ledc_conf.speed_mode = LEDC_LOW_SPEED_MODE;             /*只能使用低速模式*/
    ledc_conf.timer_sel = LEDC_PWM_TIMER;                   /*选用定时器0*/
    ledc_channel_config(&ledc_conf);

    ledc_conf.gpio_num = GPIO_NUM_4;
    ledc_conf.channel = LEDC_CHANNEL_1;
    ledc_channel_config(&ledc_conf);
}

/**
 * @brief   PWM占空比设置
 * @param   duty:占空比
 * @retval  无*/
void pwm_setduty(uint16_t duty)
{

    ledc_set_duty(LEDC_LOW_SPEED_MODE,LEDC_PWM_CH0_CHANNEL,duty);       /*设置占空比*/
    ledc_update_duty(LEDC_LOW_SPEED_MODE,LEDC_PWM_CH0_CHANNEL);         /*更新占空比*/

    ledc_set_duty(LEDC_LOW_SPEED_MODE,LEDC_CHANNEL_1,duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE,LEDC_CHANNEL_1);
}

/**
 * @brief   硬件控制PWM
 * @arg     solution:占空比分辨率
 * @arg     fre:频率
 * @retval  无*/
void pwm_hw_init(uint8_t solution, uint16_t fre)
{
    ledc_timer_config_t  timer_conf;
    timer_conf.speed_mode = LEDC_LOW_SPEED_MODE;            /*定时器模式:只能用低速模式*/
    timer_conf.clk_cfg = LEDC_USE_APB_CLK;                  /*定时器使用的时钟源*/
    timer_conf.freq_hz = fre;                               /*定时器的频率*/
    timer_conf.timer_num =  LEDC_PWM_TIMER;                 /*选择的定时器*/
    timer_conf.duty_resolution = solution;                /*定时器的占空比分辨率*/
    ledc_timer_config(&timer_conf);

    /*配置通道*/
    ledc_channel_config_t  ledc_conf;
    ledc_conf.gpio_num = LEDC_PWM_CH0_GPIO;                 /*选中所需的引脚*/
    ledc_conf.intr_type = LEDC_INTR_DISABLE;                /*禁用中断*/
    ledc_conf.channel   = LEDC_PWM_CH0_CHANNEL;             /*选中通道0*/
    ledc_conf.duty = 0;                                     /*占空比值0*/
    ledc_conf.speed_mode = LEDC_LOW_SPEED_MODE;             /*只能使用低速模式*/
    ledc_conf.timer_sel = LEDC_PWM_TIMER;                   /*选用定时器0*/
    ledc_channel_config(&ledc_conf);

    ledc_conf.gpio_num = GPIO_NUM_4;
    ledc_conf.channel = LEDC_CHANNEL_1;
    ledc_channel_config(&ledc_conf);

    ledc_fade_func_install(0);
}

/**
 * @brief   硬件PWM占空比设置
 * @param   duty:占空比
 * @retval  无*/
void pwm_hw_setdut(uint16_t duty)
{
    ledc_set_fade_with_time(LEDC_LOW_SPEED_MODE,LEDC_PWM_CH0_CHANNEL,duty,3000);
    ledc_fade_start(LEDC_LOW_SPEED_MODE,LEDC_PWM_CH0_CHANNEL,LEDC_FADE_NO_WAIT);

    ledc_set_fade_with_time(LEDC_LOW_SPEED_MODE,LEDC_PWM_CH0_CHANNEL,0,3000);
    ledc_fade_start(LEDC_LOW_SPEED_MODE,LEDC_PWM_CH0_CHANNEL,LEDC_FADE_NO_WAIT);

    ledc_set_fade_with_time(LEDC_LOW_SPEED_MODE,LEDC_CHANNEL_1,duty,3000);
    ledc_fade_start(LEDC_LOW_SPEED_MODE,LEDC_CHANNEL_1,LEDC_FADE_NO_WAIT);

    ledc_set_fade_with_time(LEDC_LOW_SPEED_MODE,LEDC_CHANNEL_1,0,3000);
    ledc_fade_start(LEDC_LOW_SPEED_MODE,LEDC_CHANNEL_1,LEDC_FADE_NO_WAIT);
}