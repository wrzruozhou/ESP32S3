//
// Created by Administrator on 2024/6/18.
//

#ifndef __LED_H
#define __LED_H

#include "driver/gpio.h"

#define LED_GPIO_PIN    GPIO_NUM_1  /* LED连接的GPIO端口 */

enum GPIO_OUTPUT_STATE
{
  PIN_RESET,
  PIN_SET
};

/* LED端口定义 */
#define LED(x)          do { x ?                                      \
                             gpio_set_level(LED_GPIO_PIN, PIN_SET) :  \
                             gpio_set_level(LED_GPIO_PIN, PIN_RESET); \
                        } while(0)  /* LED翻转 */

/* LED取反定义 */
#define LED_TOGGLE()    do { gpio_set_level(LED_GPIO_PIN, !gpio_get_level(LED_GPIO_PIN)); } while(0)  /* LED翻转 */

#define heelo gpio_config()

void led_init(void);
#endif //WRZ_LED_H
