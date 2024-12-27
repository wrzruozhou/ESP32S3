//
// Created by Administrator on 2024/9/6.
// this project was come from EXIO
//

#include "24CXX.h"
i2c_obj_t at24cxx_master;

/**
 * @brief   初始化IIC接口
 * @param   i2c_obj_t self:传入的IIC初始化参数，以判断是否已完成初始化
 * @retval  无
 * */
void at24cxx_init(i2c_obj_t self)
{
    at24cxx_master = self;
    if ( self.init_flag == ESP_FAIL )
    {
        iic_init(I2C_NUM_0);    /* init the iic*/
    }
}

/**
 * @brief   在AT24Cxx指定地址读出一个数据
 * @param   addr:开始读数的地址
 * @retval  读到的大数据
 * */
uint8_t at24cxx_read_one_byte(uint16_t addr)
{
    uint8_t  data = 0;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);                                              /**起始信号*/
    i2c_master_write_byte(cmd,AT_ADDR << 1,ACK_CHECK_EN);   /*写芯片地址*/

    i2c_master_write_byte(cmd,addr,ACK_CHECK_EN);           /**发送地址*/
    i2c_master_start(cmd);                                              /**再次发送起始信号*/
    i2c_master_write_byte(cmd, (AT_ADDR << 1)| I2C_MASTER_READ,ACK_CHECK_EN);   /**发送地址*/
    i2c_master_read_byte(cmd, &data, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(at24cxx_master.port, cmd, 1000);
    i2c_cmd_link_delete(cmd);
    vTaskDelay(10);                                                     /**等eeprom写完*/

    return data;
}

/**
 * @brief   在AT24CXX地址中写入一个数据
 * @param   addr:写入数据的目的地址
 * @param   data:要写入的数据
 * @retval
 * 无*/
void at24cxx_write_one_byte(uint16_t addr, uint8_t data)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);                                                  /**写入起始命令*/
    i2c_master_write_byte(cmd, AT_ADDR << 1,ACK_CHECK_EN);      /**发送器件的地址*/
    i2c_master_write_byte(cmd, addr, ACK_CHECK_EN);             /**发送地址*/
    i2c_master_write_byte(cmd, data, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(at24cxx_master.port, cmd, 1000);
    i2c_cmd_link_delete(cmd);
    vTaskDelay(10);
}

/**
 * @brief   检查AT24CXX是否正常
 * @note    检测原理:在器件的末地址写入0x55,然后再读取,如果读取值为0x55,则表面检测正常，否则，表示检测失败
 * @param   无
 * @retval  检测结果
 *          0:检测成功
 *          1:检测失败
 * */
uint8_t at24cxx_check(void )
{
    uint8_t temp;
    uint16_t addr = EE_TYPE;

    temp = at24cxx_read_one_byte(addr);     /**避免每次开机都写AT24CXX*/
    if ( temp == 0x55 )                     /**读取成功*/
    {
        return 0;
    } else
    {
        at24cxx_write_one_byte(addr, 0x55); /**先写入数据*/
        temp = at24cxx_read_one_byte(addr);
        if ( temp == 0x55 )
            return 0;
    }
    return 1;
}

/**
 * @brief   在AT24CXX里面的指定地址开始读出指定个数的数据
 * @param   addr:开始读出的地址    对24c02为0~255
 * @param   pbuf:数组的首地址
 * @param   datalen:要读出数据的个数
 * @retval  无
 * */
 void at24cxx_read(uint16_t addr,uint8_t *pbuf, uint16_t datalen)
{
     while ( datalen-- )
     {
         *pbuf++ = at24cxx_read_one_byte(addr++);
     }
 }

 /**
  * @brief  在AT24CXX里面的指定地址开始写入指定个数的的数据
  * @param  addr:   开始写入的地址 对24c02为0~255
  * @param  pbuf:   数据数组首地址
  * @param  datalen:    要写入数据的个数
  * @retval 无
  * */
 void  at24cxx_write(uint16_t addr, uint8_t *pbuf, uint16_t datalen)
 {
     while ( datalen-- )
     {
         at24cxx_write_one_byte(addr, *pbuf);
         addr++;
         pbuf++;
     }
 }