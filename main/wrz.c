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
#include "RGBLCD.h"

#include "freertos/FreeRTOS.h"
#include <freertos/task.h>
#include <nvs_flash.h>
#include <stdio.h>
#include <string.h>

i2c_obj_t i2c0_master;


void app_main( void )
{
    esp_err_t ret;

    led_init( );
    ret = nvs_flash_init( ); /* 初始化NVS */

    if ( ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND )
    {
        ESP_ERROR_CHECK( nvs_flash_erase( ) );
        ret = nvs_flash_init( );
    }
    i2c0_master = iic_init( I2C_NUM_0 );
    xl9555_init( i2c0_master );  /**初始化IO拓展芯片*/
    at24cxx_init( i2c0_master ); /**初始化24CXX*/
    ap3216c_init( i2c0_master );
    ap3216c_Int();
    ltdc_init();


        while ( 1 )
        {
            ltdc_clear(WHITE);
            ltdc_draw_line(0, 0, 500, 100, YELLOW);
            ltdc_draw_line(70, 89, 123, 121, RED);
            ltdc_draw_line(100, 100, 162, 100, BLACK);
            ltdc_draw_rectangle(100,100,600,300,GREEN);
            ltdc_show_num(400,216, 105, 6, 32, GREEN);
            ltdc_show_xnum(315,179,12,4,32,0x80,RED);
            ltdc_show_string(30,70,550,150,32,"the time is 2024 - 10 - 23dsadadafafef faf aew wef wef awf daf afearasdasdawe aefaeadsadwafa", CYAN);
            LED_TOGGLE();
            vTaskDelay( 500 );
        }
}