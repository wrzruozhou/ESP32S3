/**
 * @brief   这个项目是根据8.MQTT工程改编的
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
//#include "tcp_mclient.h"
#include "tcp_mserver.h"
#include "mqtt_test.h"

#include "freertos/FreeRTOS.h"
#include <freertos/task.h>
#include <nvs_flash.h>
#include <stdio.h>
#include <string.h>
#include "http_weather.h"
#include "gt1151.h"
#include "touch.h"
#include "lvgl_demo.h"

#include <lvgl_mytest.h>

/* 6个触控点的颜色(电容触摸屏用) */
static const uint16_t POINT_COLOR_TBL[6] = {
        RED,
        GREEN,
        BLUE,
        YELLOW,
        MAGENTA,
        CYAN,
};

/**
 * @brief   电容触摸屏测试函数
 * @param   无
 * @retval  无
 * */
void ctp_test(void )
{
    uint8_t t = 0;
    uint8_t i = 0;
    uint16_t lastpos[10][2];

    while ( 1 )
    {
        tp_dev.scan(0);

        for ( i = 0; i < 5; ++i )
        {
            if ( tp_dev.sta & (1 << i) )
            {
                if ( (tp_dev.x[i] < ltdcdev.width) && (tp_dev.y[i] < 480) )
                {
                    if ( lastpos[i][0] == 0xffff )                                          /*上一个点的数值超出了范围*/
                    {
                        lastpos[i][0] = tp_dev.x[i];
                        lastpos[i][1] = tp_dev.y[i];
                    }
                    lcd_draw_bline(lastpos[i][0], lastpos[i][1], tp_dev.x[i], tp_dev.y[i], 2, POINT_COLOR_TBL[i]);
                    lastpos[i][0] = tp_dev.x[i];
                    lastpos[i][1] = tp_dev.y[i];

                    if (tp_dev.x[i] > (ltdcdev.width - 24) && tp_dev.y[i] < 20)
                    {
                        load_draw_dialog();                                                                         /* 清除 */
                    }
                }
            }
            else{
                lastpos[i][0] = 0xffff;
            }
        }
        vTaskDelay(5);
        t++;
        if (t % 20 == 0)
        {
            LED_TOGGLE();
        }
    }
}


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

/**
 * Weather_TASK 任务配置
 * */
#define Weather_TASK_PRIO           11         /* 任务优先级 */
#define Weather_STK_SIZE            8192        /* 任务堆栈大小 */
TaskHandle_t WeatherTask_Handler;               /* 任务句柄 */

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


    vTaskDelay(10);
    i2c0_master = iic_init(I2C_NUM_0);

    vTaskDelay(10);
    xl9555_init(i2c0_master);  /**初始化IO拓展芯片*/
//    at24cxx_init( i2c0_master ); /**初始化24CXX*/
//    ltdc_init();
    led_init();
//
//    gt9xxx_init();
    lvgl_demo();
//    tp_dev.init();                      /* 初始化触摸屏 */
//    load_draw_dialog();
//    ctp_test();
//    wifi_sta_init();


//    taskENTER_CRITICAL(&my_spinlock);
    /* key任务 */
//    xTaskCreate((TaskFunction_t)key_task,
//        (const char*)"key_task",
//        (uint16_t)KEY_STK_SIZE,
//        (void*)NULL,
//        (UBaseType_t)KEY_TASK_PRIO,
//        (TaskHandle_t*)&KEYTask_Handler);
//    if ( wifi_connect_ok_flag == 1 )
//    {
//        xTaskCreate((TaskFunction_t)http_test_task,
//                     (const char*)"http_test_task",
//                     (uint16_t)Weather_STK_SIZE,
//                     (void*)NULL,
//                     (UBaseType_t)Weather_TASK_PRIO,
//                     (TaskHandle_t*)&WeatherTask_Handler);
//    }
    /* LED测试任务 */
//    xTaskCreate((TaskFunction_t)led_task,
//        (const char*)"led_task",
//        (uint16_t)LED_STK_SIZE,
//        (void*)NULL,
//        (UBaseType_t)LED_TASK_PRIO,
//        (TaskHandle_t*)&LEDTask_Handler);
//    taskEXIT_CRITICAL(&my_spinlock);

//    mqtt_demo();
    while(1)
    {
        vTaskDelay(1000);
        LED_TOGGLE();
    }


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
//        key = xl9555_key_scan(0);
//        if (KEY0_PRES == key)
//        {
//            g_lwip_send_flag_server |= LWIP_SEND_DATA; /* 标记LWIP有数据要发送 */
//        }
//    g_publish_flag |= LWIP_SEND_DATA;
        g_publish_flag |= 1;

        vTaskDelay(10000);
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

