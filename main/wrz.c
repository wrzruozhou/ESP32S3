#include "ESPTIMER.h"
#include "GPTIMER.h"
#include "KEY.h"
#include "LED.h"
#include "SWPWM.h"
#include "UART.h"
#include "freertos/FreeRTOS.h"
#include <freertos/task.h>
#include <nvs_flash.h>
#include <stdio.h>
#include <string.h>

void app_main(void)
{
    esp_err_t ret;
    uint8_t dir = 1;
    uint16_t ledpwmval = 0;

    ret = nvs_flash_init();     /* 初始化NVS */

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    led_init();
    key_init();
    pwm_init(10,20000);

    while (1)
    {
        if (dir == 1)
        {
            ledpwmval += 5; /* dir==1 ledpwmval递增 */
        }
        else
        {
            ledpwmval -= 5; /* dir==0 ledpwmval递减 */
        }

        if (ledpwmval > 1005)
        {
            dir = 0;        /* ledpwmval到达1005后，方向为递减 */
        }

        if (ledpwmval < 5)
        {
            dir = 1;        /* ledpwmval递减到5后，方向改为递增 */
        }

        /* 设置占空比 */
        pwm_setduty(ledpwmval);
        vTaskDelay(10);
    }
}