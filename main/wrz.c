#include "ESPTIMER.h"
#include "GPTIMER.h"
#include "KEY.h"
#include "LED.h"
#include "SWPWM.h"
#include "UART.h"
#include "XL9555.h"
#include "freertos/FreeRTOS.h"
#include <freertos/task.h>
#include <nvs_flash.h>
#include <stdio.h>
#include <string.h>
#include <SB_RTC.h>

i2c_obj_t i2c0_master;
char* weekdays[]={"Sunday","Monday","Tuesday","Wednesday",
                     "Thursday","Friday","Saterday"};
/**
 * @brief   串口信息实验
 * @param   无
 * @param   无
 * */
void show_mesg( void ) {
    /* 串口输出实验信息 */
    printf( "\n" );
    printf( "********************************" );
    printf( "ESP32-S3\n" );
    printf( "EXIO TEST\n" );
    printf( "KEY0:Beep On,KEY1:Beep Off\n" );
    printf( ":KEY2:LED On,KEY3:LED Off\n" );
    printf( "********************************" );
    printf( "\n" );
}

void app_main( void ) {
    printf("led off1\n");
    esp_err_t ret;
    uint8_t key;
    uint8_t tbuf[40];

    ret = nvs_flash_init( ); /* 初始化NVS */

    if ( ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND ) {
        ESP_ERROR_CHECK( nvs_flash_erase( ) );
        ret = nvs_flash_init( );
    }

    led_init( );
    i2c0_master = iic_init( I2C_NUM_0 );
    xl9555_init( i2c0_master );
    key_init( );

    rtc_set_time(2023,1,26,12,12,12);   /* 设置RTC时间 */


    while ( 1 ) {
        rtc_get_time();
        sprintf((char *)tbuf, "Time:%02d:%02d:%02d", calendar.hour, calendar.min, calendar.sec);
        printf("Time:%02d:%02d:%02d\r\n", calendar.hour, calendar.min, calendar.sec);
        sprintf((char *)tbuf, "Date:%04d-%02d-%02d", calendar.year, calendar.month, calendar.date);
        printf("Date:%02d-%02d-%02d\r\n",  calendar.year,  calendar.month,  calendar.date);
        sprintf((char *)tbuf, "Week:%s", weekdays[calendar.week - 1]);
        LED_TOGGLE();
        vTaskDelay(1000);
    }
}