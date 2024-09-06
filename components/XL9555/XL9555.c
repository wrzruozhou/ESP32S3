//
// Created by Administrator on 2024/6/29.
//

#include "XL9555.h"

i2c_obj_t xl9555_i2c_master;
static uint16_t xl9555_failed = 0;

/**
 * @brief   读取XL9555的16位IO值
 * @param   data:读取数据的存储区
 * @param   len:读取数据的大小
 * @retval  ESP_OK:读取成功；其他:失败
 * */
esp_err_t xl9555_read_byte(uint8_t *data, size_t len)
{
    uint8_t memaddr_buf[1];
    memaddr_buf[0] = XL9555_INPUT_PORT0_REG;

    i2c_buf_t bufs[2] = {
            {.len = 1, .buf = memaddr_buf},
            {.len = len,. buf = data},
    };
    return i2c_transfer(&xl9555_i2c_master,XL9555_ADDR,2,bufs,I2C_FLAG_READ|I2C_FLAG_WRITE|I2C_FLAG_STOP);
}

/**
 * @brief   向XL9555写入16位IO值
 * @param   reg:寄存器地址
 * @param   data:要写入的数据
 * @param   len:要写入数据的大小
 * @retval  ESP_OK:读取成功；其他：读取失败
 * */
esp_err_t  xl9555_write_byte(uint8_t reg, uint8_t *data,size_t len)
{
     i2c_buf_t bufs[2] = {
         {.len = 1, .buf = &reg},
         {.len = len, .buf = data},
     };
     return i2c_transfer(&xl9555_i2c_master,XL9555_ADDR,2,bufs,I2C_FLAG_STOP);
 }
 /**
  * @brief  控制某个IO的电平
  * @param  pin:控制的IO
  * @param  val:电平
  * @retval 返回所有的IO状态
 * */
uint16_t xl9555_pin_write(uint16_t pin, int val) {
    uint8_t w_data[ 2 ];
    uint16_t temp = 0x0000;

    xl9555_read_byte( w_data, 2 ); /*先读取原先的电平*/

    if ( pin <= GBC_KEY_IO ) {
        if ( val ) {
            w_data[ 0 ] |= ( uint8_t ) ( 0xff & pin );
        } else {
            w_data[ 0 ] &= ~( uint8_t ) ( 0xff & pin );
        }
    } else {
        if ( val ) {
            w_data[ 1 ] |= ( uint8_t ) ( 0xff & ( pin >> 8 ) );
        } else {
            w_data[ 1 ] &= ~( uint8_t ) ( 0xff & ( pin >> 8 ) );
        }
    }
    temp = ( ( uint16_t ) w_data[ 1 ] << 8 ) | w_data[ 0 ];

    xl9555_write_byte( XL9555_OUTPUT_PORT0_REG, w_data, 2 );

    return temp;
}

/**
 * @brief   读取某个IO状态
 * @param   pin 要获取状态的IO
 * @retval  此IO口的值(状态，0、1)
 * */
int xl9555_pin_read(uint16_t pin)
{
    uint16_t ret;
    uint8_t r_data[2];
    xl9555_read_byte(r_data, 2);
    ret = r_data[1] << 8 | r_data[0];
    return (ret & pin) ? 1 : 0;
}

/**
 * @brief   xl9555的IO配置
 * @param   config_value:IO配置输入或者输出
 * @retval  返回设置的数值
 * */
uint16_t xl9555_ioconfig(uint16_t config_value)
{
    /* 从机地址 + CMD +data1(P0) + data2(P1) */
    /*P0 1 2 3为高电平时候 0000 0000 0000 1111*/

    uint8_t  data[2];
    esp_err_t  err;
    int retry = 3;

    data[0] = (uint8_t)(0xFF & config_value);
    data[1] = (uint8_t)(0xff & ( config_value >> 8 ));

    do {
        err = xl9555_write_byte(XL9555_CONFIG_PORT0_REG,data,2);

        if ( err != ESP_OK )
        {
            retry--;
            vTaskDelay(100);
            ESP_LOGE("IIC","%s configure %x failed, ret: %d",__func__ , config_value, err);
            xl9555_failed = 1;

            if ( (retry <= 0) && xl9555_failed )
            {
                vTaskDelay(5000);
                esp_restart();
            }
        }
        else
        {
            xl9555_failed = 0;
            break;
        }
    }while(retry);

    return config_value;
}

/**
 * @brief   初始化XL9555
 * @param   无
 * @param   无
 * */
void xl9555_init(i2c_obj_t self)
{
    uint8_t r_data[2];
    gpio_config_t  pGPIOConfig;
    if ( self.init_flag == ESP_FAIL )
    {
        iic_init(I2C_NUM_0);        //初始化IIC
    }
    xl9555_i2c_master = self;

    pGPIOConfig.mode = GPIO_MODE_INPUT;
    pGPIOConfig.intr_type = GPIO_INTR_DISABLE;
    pGPIOConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
    pGPIOConfig.pull_up_en = (1ull << GPIO_PULLUP_ENABLE);
    pGPIOConfig.pin_bit_mask = XL9555_INT_IO;
    gpio_config(&pGPIOConfig);                      /*配置XL_INT引脚*/

    /*上电先读取一次清除中断标志*/
    xl9555_read_byte(r_data, 2);

    xl9555_ioconfig(0xf003);

    xl9555_pin_write(BEEP_IO,1);
    xl9555_pin_write(SPK_EN_IO,1);
}

/**
 * @brief       按键扫描函数
 * @param       mode:0->不连续;1->连续
 * @retval      键值, 定义如下:
 *              KEY0_PRES, 1, KEY0按下
 *              KEY1_PRES, 2, KEY1按下
 *              KEY2_PRES, 3, KEY2按下
 *              KEY3_PRES, 4, KEY3按下
 */
 uint8_t xl9555_key_scan(uint8_t mode)
{
     uint8_t  keyval = 0;
     static uint8_t key_up = 1;             /*按键松开标志*/

     if ( mode )
     {
         key_up = 1;                        /*支持连按*/
     }

     if ( key_up && (KEY0 == 0 || KEY1 == 0 || KEY2 == 0 || KEY3 == 0) )        /*按键松开标志为1，且有任意一个按键按下了*/
     {
         vTaskDelay(10);            /*去抖*/
        key_up = 0;
        if ( KEY0 == 0 )
            keyval = KEY0_PRES;
        if ( KEY1 == 0 )
            keyval = KEY1_PRES;
        if ( KEY2 == 0 )
            keyval = KEY2_PRES;
        if ( KEY3 == 0)
            keyval = KEY3_PRES;
     } else if( KEY0 == 1 && KEY1 == 1 && KEY2 == 1 && KEY3 == 1 )              /*没有任何按键按下*/
     {
         key_up = 1;
     }
    return keyval;                                                                 /*返回键值*/
 }