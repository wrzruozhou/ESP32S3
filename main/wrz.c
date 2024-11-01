/**
 * @brief   这个项目是根据7.2WIFI_AP工程改编的
 * */

#include "ESPTIMER.h"
#include "GPTIMER.h"
#include "KEY.h"
#include "LED.h"
#include "SWPWM.h"
#include "UART.h"
#include "XL9555.h"
#include "24CXX.h"
#include "ADCM.h"
#include "AP3216C.h"
#include "sensor.h"
#include "RGBLCD.h"
#include "wifi_sta.h"

#include "UDP_CONFIG.h"
#include "UDP_EXAMPLE.h"

#include "freertos/FreeRTOS.h"
#include <freertos/task.h>
#include <nvs_flash.h>
#include <stdio.h>
#include <string.h>

 /*FreeRTOS配置*/
 /* LED_TASK 任务 配置
  * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
  */
#define LED_TASK_PRIO          10        /* 任务优先级 */
#define LED_STK_SIZE            2048        /* 任务堆栈大小 */
TaskHandle_t LEDTask_Handler;               /* 任务句柄 */
void led_task(void* pvParameters);          /* 任务函数 */

/* KEY_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define KEY_TASK_PRIO           11         /* 任务优先级 */
#define KEY_STK_SIZE            2048        /* 任务堆栈大小 */
TaskHandle_t KEYTask_Handler;               /* 任务句柄 */
void key_task(void* pvParameters);          /* 任务函数 */

static portMUX_TYPE my_spinlock = portMUX_INITIALIZER_UNLOCKED;
i2c_obj_t i2c0_master;


void app_main(void)
{

    esp_err_t ret;

    ret = nvs_flash_init(); /* 初始化NVS */

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    led_init();
    vTaskDelay(10);
    i2c0_master = iic_init(I2C_NUM_0);
    vTaskDelay(10);
    xl9555_init(i2c0_master);  /**初始化IO拓展芯片*/
    at24cxx_init( i2c0_master ); /**初始化24CXX*/
    ltdc_init();
    ltdc_show_string(0, 0, 240, 32, 32, "ESP32-S3", RED);
    ltdc_show_string(0, 40, 240, 24, 24, "WiFi UDP Test", RED);
    ltdc_show_string(0, 70, 240, 16, 16, "ATOM@ALIENTEK", RED);
    wifi_sta_init();

    taskENTER_CRITICAL(&my_spinlock);
    /* key任务 */
    xTaskCreate((TaskFunction_t)key_task,
        (const char*)"key_task",
        (uint16_t)KEY_STK_SIZE,
        (void*)NULL,
        (UBaseType_t)KEY_TASK_PRIO,
        (TaskHandle_t*)&KEYTask_Handler);
    /* LED测试任务 */
    xTaskCreate((TaskFunction_t)led_task,
        (const char*)"led_task",
        (uint16_t)LED_STK_SIZE,
        (void*)NULL,
        (UBaseType_t)LED_TASK_PRIO,
        (TaskHandle_t*)&LEDTask_Handler);
    taskEXIT_CRITICAL(&my_spinlock);

    lwip_demo();            /* lwip测试代码 */


}

/**
 * @brief       key_task
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void key_task(void* pvParameters)
{
    pvParameters = pvParameters;

    uint8_t key;

    while (1)
    {
        key = xl9555_key_scan(0);
        if (KEY0_PRES == key)
        {
            g_lwip_send_flag |= LWIP_SEND_DATA; /* 标记LWIP有数据要发送 */
        }

        vTaskDelay(100);
    }
}

/**
 * @brief       系统再运行
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void led_task(void* pvParameters)
{
    pvParameters = pvParameters;
    while (1)
    {
        LED_TOGGLE();
        vTaskDelay(500);
    }
}

