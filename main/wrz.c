/**
 * @brief   这个项目是根据EEPROM工程改编的
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

#include "freertos/FreeRTOS.h"
#include <freertos/task.h>
#include <nvs_flash.h>
#include <stdio.h>
#include <string.h>

i2c_obj_t i2c0_master;


void app_main( void )
{
    float temp;
    esp_err_t ret;

    ret = nvs_flash_init( ); /* 初始化NVS */

    if ( ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND )
    {
        ESP_ERROR_CHECK( nvs_flash_erase( ) );
        ret = nvs_flash_init( );
    }

    led_init( );
    i2c0_master = iic_init( I2C_NUM_0 );
    xl9555_init( i2c0_master );  /**初始化IO拓展芯片*/
    at24cxx_init( i2c0_master ); /**初始化24CXX*/
    ap3216c_init( i2c0_master );
    temperature_sensor_init();

        while ( 1 )
        {
            temp = sensor_get_temperature();
            printf("当前的温度是%.2lf\n", temp);
            vTaskDelay( 1000 );
        }
}