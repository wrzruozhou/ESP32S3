//
// Created by Administrator on 2024/9/11.
//

#include "AP3216C.h"

i2c_obj_t ap3216c_master;

/**
 * @brief   初始化外部中断引脚
 * @param   无
 * @retval  无
 * */
void ap3216c_Int(void )
{
    gpio_config_t pGPIOConfig;
    pGPIOConfig.mode = GPIO_MODE_INPUT;
    pGPIOConfig.intr_type = GPIO_INTR_POSEDGE;
    pGPIOConfig.pull_up_en = GPIO_PULLUP_DISABLE;
    pGPIOConfig.pull_down_en = GPIO_PULLDOWN_ENABLE;
    pGPIOConfig.pin_bit_mask = AP_INT_IO;
    gpio_config(&pGPIOConfig);
}

/**
 * @brief   读取ap3216c的16位IO值
 * @param   data:存储区
 * @param   reg:寄存器
 * @retval  ESP_OK:读取成功;其他;读取失败
 * */
static esp_err_t ap3216c_read_one_byte(uint8_t* data,uint8_t reg)
{
    uint8_t reg_buf[1];

    reg_buf[0] = reg;
    i2c_buf_t buf[2] = {
        {.len = 1, .buf = reg_buf},
        {.len = 1, .buf = data},
    };
    i2c_transfer(&ap3216c_master,AP3216C_ADDR,2,buf,I2C_FLAG_WRITE|I2C_FLAG_READ|I2C_FLAG_STOP);
    return ESP_OK;
}

/**
 * @brief   向ap3216c写入16位IO值
 * @param   reg:写入的寄存器
 * @param   data:要写入的数据
 * @retval  ESP_OK:读取成功，其他，失败值
 * */
static esp_err_t ap3216c_write_one_byte(uint8_t reg, uint8_t data)
{
    uint8_t reg_buf[1];
    reg_buf[0] = reg;
    i2c_buf_t buf[2] = {
        {.len = 1, .buf = reg_buf},
        {.len = 1, .buf = &data}
    };

    i2c_transfer(&ap3216c_master, AP3216C_ADDR, 2, buf, I2C_FLAG_STOP);
    return ESP_OK;
}

/**
 * @brief   配置ap3216c
 * @param   无
 * @retval  0,成功
 *          1，失败
 * */
uint8_t ap3216c_config(void )
{
    uint8_t temp;

    ap3216c_write_one_byte(0x00, 0x04);     /**复位AP3216C*/
    vTaskDelay(50);                         /**复位至少要保持10ms*/

    /**配置ALS可照到的动态范围*/
    ap3216c_write_one_byte(0x10,0x3F);      /**检测范围是323lux，60次转化一次中断*/

    /**配置PS & IR*/
    ap3216c_write_one_byte(0x20,0xf7);      /**满精度, 增益, 8次转换触发一次*/

    /**设置光照强度的阈值*/
    ap3216c_write_one_byte(0x1a,0x00);
    ap3216c_read_one_byte(&temp, 0x1a);
    if ( temp ==  0x00){
        ESP_LOGI("ap3216c","低阈值低位写入成功\n");
    }
    else{
        ESP_LOGE("ap3216c","低阈值低位写入失败\n");
    }
    ap3216c_write_one_byte(0x1b,0xf0);
    ap3216c_read_one_byte(&temp, 0x1b);
    if ( temp == 0xf0 ){
        ESP_LOGI("ap3216c","低阈值高位写入成功\n");
    }else{
        ESP_LOGE("ap3216c","低阈值高位写入失败\n");
    }

    /**设置接近传感器的阈值*/


    ap3216c_write_one_byte(0x00, 0x03);         /*开启ALS、PS+IR*/
    ap3216c_read_one_byte(&temp, 0x00);         /*读取刚刚写进去的0x03*/
    if ( temp == 0x03 )
    {
        ESP_LOGI("ap3216c", "ap3216c write success!!!\n");
        return 0;
    }
    else
    {
        ESP_LOGE("ap3216c", "ap3216c write faild!!!\n");
        return 1;
    }



}


/**
 * @brief   初始化AP3216C
 * @param   无
 * @retval  无
 * */
void ap3216c_init(i2c_obj_t self)
{
    if ( self.init_flag == ESP_FAIL )
    {
        iic_init(I2C_NUM_0);
    }
    ap3216c_master = self;
    while ( ap3216c_config() )      /** 检测不到AP3216C */
    {
        ESP_LOGE("ap3216c", "ap3216c init fail!!!\n");
        vTaskDelay(1000);
    }
}

/**
 * @brief   读取ap3216c的数据
 * @note    读取原始数据,ALS,PS和IR
 *          如果同时打开ALS,IR+PS的画，两次数据读取的时间间隔就要大于112.5ms
 * @param   ir:IR传感器指针值         10位 红外检测
 * @param   ps:PS传感器指针值         10位 接近检测
 * @param   als:ALS传感器指针值       16位 环境光检测
 * @retval  无
 * */
void ap3216c_read_data(uint16_t *ir, uint16_t *ps, uint16_t *als)
{
    uint8_t buf[6] = {0};
    uint8_t i;

    for (i = 0; i < 6; ++i )
    {
        ap3216c_read_one_byte(&buf[i],0x0a+i);
    }
    if ( buf[0] & 0x80 )
    {
        *ir = 0;                /**数据非法*/
    }else{
        *ir = ((uint16_t )buf[1] << 2) | (buf[0] & 0x03);
    }

    *als = ((uint16_t)buf[3] << 8) | buf[2];
    if ( buf[4] & 0x80 )
    {
        ESP_LOGE("ps","ps 超过阈值 有人靠近\n");
    }

    if ( buf[4] & 0x40 )
    {
        *ps = 0;
    }else{
        *ps = ( (uint16_t)(buf[5] & 0x3F << 4) | (buf[4] & 0x0F) );
    }

}