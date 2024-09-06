#include "ESPTIMER.h"
#include "GPTIMER.h"
#include "KEY.h"
#include "LED.h"
#include "SWPWM.h"
#include "UART.h"
#include "XL9555.h"
#include "24CXX.h"
#include "freertos/FreeRTOS.h"
#include <freertos/task.h>
#include <nvs_flash.h>
#include <stdio.h>
#include <string.h>

i2c_obj_t i2c0_master;

const uint8_t g_text_buf[] = {"中国男足不要碧脸"};  /**要写入的字符串*/
#define TEXT_SIZE sizeof(g_text_buf)                /**要写入字符串的长度*/

/**
 * @brief   显示实验信息
 * @param   无
 * @retval  无
 * */
 void show_mesg(void )
{
     /* 串口输出实验信息 */
     printf("\n");
     printf("********************************\n");
     printf("ESP32\n");
     printf("IIC EEPROM TEST\n");
     printf("ATOM@ALIENTEK\n");
     printf("KEY0:Write Data, KEY1:Read Data\n");
     printf("********************************\n");
     printf("\n");
 }

void app_main( void ) {
    uint16_t i = 0;
    uint8_t err = 0;
    uint8_t key;
    uint8_t datatemp[TEXT_SIZE];
    esp_err_t ret;

    ret = nvs_flash_init( ); /* 初始化NVS */

    if ( ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND ) {
        ESP_ERROR_CHECK( nvs_flash_erase( ) );
        ret = nvs_flash_init( );
    }

    led_init( );
    i2c0_master = iic_init( I2C_NUM_0 );
    xl9555_init( i2c0_master );             /**初始化IO拓展芯片*/
    at24cxx_init(i2c0_master);              /**初始化24CXX*/
    show_mesg();                                /**显示试验信息*/

    err = at24cxx_check();                      /**检测at24c02*/
    if ( err != 0 )
    {
        while ( 1 )
        {
            printf("24c02 check failed, please check!\n");
            vTaskDelay(500);
            LED_TOGGLE();
        }
    }

    printf("24c02 Ready!\n");
    printf("\n");

    while ( 1 )
    {
        key = xl9555_key_scan(0);
        switch ( key ) {
            case KEY0_PRES:
                at24cxx_write(0,(uint8_t *)g_text_buf,TEXT_SIZE);
                printf("the data written is :%s\n",g_text_buf);
                break;
            case KEY1_PRES:
                at24cxx_read(0,datatemp,TEXT_SIZE);
                printf("the data read is:%s\n",datatemp);
                break;
            default:
                break;
        }
        i++;
        if ( i == 20 )
        {
            LED_TOGGLE();
            i = 0;
        }
        vTaskDelay(10);
    }
}