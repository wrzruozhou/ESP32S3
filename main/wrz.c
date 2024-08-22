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

i2c_obj_t i2c0_master;

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

    ret = nvs_flash_init( ); /* 初始化NVS */

    if ( ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND ) {
        ESP_ERROR_CHECK( nvs_flash_erase( ) );
        ret = nvs_flash_init( );
    }

    led_init( );
    i2c0_master = iic_init( I2C_NUM_0 );
    xl9555_init( i2c0_master );
    key_init( );

    while ( 1 ) {

        key = xl9555_key_scan( 0 );
        switch ( key ) {
            case KEY0_PRES: {
                printf( "KEY0 has been pressed \n" );
                xl9555_pin_write( BEEP_IO, 0 );
                break;
            }
            case KEY1_PRES: {
                printf( "KEY1 has been pressed \n" );
                xl9555_pin_write( BEEP_IO, 1 );
                break;
            }
            case KEY2_PRES: {
                printf( "KEY2 has been pressed \n" );
                LED( 0 );
                break;
            }
            case KEY3_PRES: {
                printf( "KEY3 has been pressed \n" );
                LED( 1 );
                break;
            }
            default:
                break;
        }

        if ( XL9555_INT == 0 ) {
            printf( "123" );
        }
        vTaskDelay(10);
    }
}