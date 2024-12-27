//
// Created by Administrator on 2024/6/18.
//

#include "LED.h"

/**
 * @brief   初始化LED
 * @param   无
 * @retval  无
 * */
 void led_init(void)
{
    gpio_config_t gpio_init_struct = {0};
    gpio_init_struct.mode =GPIO_MODE_INPUT_OUTPUT;
    gpio_init_struct.intr_type = GPIO_INTR_DISABLE;
    gpio_init_struct.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_init_struct.pull_up_en = GPIO_PULLUP_ENABLE;
    //设置引脚的位掩码
    gpio_init_struct.pin_bit_mask = 1ull<<GPIO_NUM_1;
    gpio_config(&gpio_init_struct);
    LED(1);
}





