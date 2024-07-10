//
// Created by Administrator on 2024/6/18.
//

#ifndef __LED_H
#define __LED_H

#include "driver/gpio.h"

enum GPIO_OUTPUT_STATE
{
  PIN_RESET,
  PIN_SET
};

#define LED(x)  do{ \
    x? gpio_set_level(GPIO_NUM_1,PIN_RESET): \
      gpio_set_level(GPIO_NUM_1,PIN_SET);     \
  }while(0)

#define LED_TOGGLE() do{ \
    gpio_set_level(GPIO_NUM_1,!gpio_get_level(GPIO_NUM_1)); \
  }while(0)

#define heelo gpio_config()

void led_init(void);
#endif //WRZ_LED_H
