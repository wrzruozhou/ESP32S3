/**
 * @brief   这个项目是根据4.2三轴传感器工程改编的
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
#include "qma6100a.h"

i2c_obj_t i2c0_master;


void app_main( void )
{
    uint8_t t = 0;
    esp_err_t ret;
    qma6100p_rawdata_t xyz_rawdata;

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
    qma6100p_init(i2c0_master);

        while ( 1 )
        {
            vTaskDelay(10);
            t++;

            if (t == 20)                    /* 0.2秒左右更新一次三轴原始值 */
            {
                qma6100p_read_rawdata(&xyz_rawdata);
                printf("the data of the accx is %f\n",xyz_rawdata.acc_x);
                printf("the date of the accy is %f\n",xyz_rawdata.acc_y);
                printf("the date of the accz is %f\n",xyz_rawdata.acc_z);
                printf("the date of the accg is %f\n",xyz_rawdata.acc_g);
                t = 0;
                LED_TOGGLE();
            }
        }
}