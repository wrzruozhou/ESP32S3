//
// Created by Administrator on 2024/6/19.
//

#include "KEY.h"

/**
 * @brief   无中断初始化按键
 * @param   无
 * @retval  无
 */
void key_init(void)
{
  gpio_config_t pGPIOConfig;
  pGPIOConfig.mode = GPIO_MODE_INPUT;
  pGPIOConfig.pull_up_en = GPIO_PULLUP_ENABLE;
  pGPIOConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
  pGPIOConfig.intr_type = GPIO_INTR_DISABLE;
  pGPIOConfig.pin_bit_mask = 1ull<<GPIO_NUM_0;          /*boot的引脚是这个*/
  gpio_config(&pGPIOConfig);
}

/**
* @brief    按键扫描函数
 * @param   mode
 *  @arg    1:启用连续扫描，每次都会返回键值
 *  @arg    0:不使用连续扫描，按下不放开，只会返回一次键值
*  @retval  返回的键值
*/
uint8_t key_scan(uint8_t mode)
{
  uint8_t keyval = 0;
  static uint8_t key_boot = 1;    /*按键松开标志*/

  if (mode)
  {
    key_boot = 1;
  }
  if (key_boot && (BOOT == 0))
  {
    vTaskDelay(10);
    key_boot = 0;
    if (BOOT==0)
    {
      keyval = 1;
    }
  } else if (BOOT == 1)
  {
      key_boot = 1;   /*标识已经松开*/
  }
  return keyval;
}
/*-----------------------------------------------------------------------------------------------------------------*/
/**
 * @brief   这个是按键的中断回调函数
 * @param   传入的数据
 * @arg 中断回填传入的参数
 * @retval  无*/
static IRAM_ATTR void exti_gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    if (gpio_num == GPIO_NUM_0)
    {
        LED_TOGGLE();
    }
}
/**
 * @brief   按键中断初始化
 * @param   无
 * @retval  无
 * */
void key_it_init(void)
{
    gpio_config_t pGPIOConfig;
    pGPIOConfig.mode = GPIO_MODE_INPUT;
    pGPIOConfig.intr_type = GPIO_INTR_NEGEDGE;
    pGPIOConfig.pull_up_en = GPIO_PULLUP_ENABLE;
    pGPIOConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
    pGPIOConfig.pin_bit_mask = GPIO_NUM_0;
    /*初始化GPIO*/
    gpio_config(&pGPIOConfig);

    /*初始化中断服务函数*/
    gpio_install_isr_service(0);

    /*设置GPIO的中断回调函数*/
    gpio_isr_handler_add(GPIO_NUM_0,exti_gpio_isr_handler,(void *)GPIO_NUM_0);
}