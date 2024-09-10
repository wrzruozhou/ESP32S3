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

#include "freertos/FreeRTOS.h"
#include <freertos/task.h>
#include <nvs_flash.h>
#include <stdio.h>
#include <string.h>

i2c_obj_t i2c0_master;


void app_main( void )
{
    uint16_t adcdata;
    float voltage;
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
#if USE_IDF_V5
    adc_init();
#else
    adc_init_v5();
#endif
        while ( 1 )
        {
#if USE_IDF_V5
            adcdata = adc_get_result_average(ADC_ADCX_CHY, 20);
#else
            adcdata = adc_get_result_average_V5(ADC_CHANNEL_7, 20);
#endif
            voltage = (float )adcdata *(3.3 / 4095);
            printf("the voltage is %.2f\n", voltage);

            vTaskDelay( 1000 );
        }
}