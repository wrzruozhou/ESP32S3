//
// Created by Administrator on 2024/6/29.
//
#include "IIC.h"

i2c_obj_t iic_master[I2C_NUM_MAX];

/**
 * @brief   初始化IIC
 * @param   iic_pore:iic的编号
 * @retval  IIC控制块0 / IIC控制块1
 * */
i2c_obj_t iic_init(uint8_t iic_port)
{
    uint8_t i;
    i2c_config_t  i2c_conf;
    if (iic_port==I2C_NUM_0){
        i = 0;
    } else{
        i = 1;
    }

    iic_master[i].port = iic_port;
    iic_master[i].init_flag = ESP_FAIL;                 /*通用错误标识*/

    if (iic_master[i].port == I2C_NUM_0)
    {
        iic_master[i].scl = IIC0_SCL_GPIO_PIN;
        iic_master[i].sda = IIC0_SDA_GPIO_PIN;
    }
    else
    {
        iic_master[i].scl = IIC1_SCL_GPIO_PIN;
        iic_master[i].sda = IIC1_SDA_GPIO_PIN;
    }

    i2c_conf.mode = I2C_MODE_MASTER;                    /*设置IIC模式 主从模式*/
    i2c_conf.scl_io_num = iic_master[i].scl;            /*设置IIC SCL引脚*/
    i2c_conf.sda_io_num = iic_master[i].sda;            /*设置IIC SDA引脚*/
    i2c_conf.scl_pullup_en = GPIO_PULLUP_ENABLE;        /*使能SCL引脚上拉*/
    i2c_conf.sda_pullup_en = GPIO_PULLUP_ENABLE;        /*使能SDA引脚上拉*/
    i2c_conf.master.clk_speed = IIC_FREQ;               /*设置IIC的通信速率*/
    i2c_param_config(iic_master[i].port,&i2c_conf);

    /*激活IIC的控制器驱动*/
    iic_master[i].init_flag = i2c_driver_install(iic_master[i].port,                    /*端口号*/
                                                    i2c_conf.mode,                              /*主机模式*/
                                                    I2C_MASTER_RX_BUF_DISABLE,      /*从机模式下接收缓存大小(主机模式不使用)*/
                                                    I2C_MASTER_TX_BUF_DISABLE,      /*从机模式下发送缓存大小(从机模式不使用)*/
                                                    0);                             /*用于分配中断的标志*/

    if ( iic_master[i].init_flag != ESP_OK )
    {
        while(1)
        {
            printf("%s , ret: %d",__func__ ,iic_master[i].init_flag);
            vTaskDelay(1000);
        }
    }

    return iic_master[i];
}

/**
 * @brief   IIC读写数据
 * @note    写是低电平，读是高电平
 * @param   self:设备控制块
 * @param   addr:设备地址
 * @param   n:数据大小
 * @param   bufs:要发送的数据或者是要读取的存储区               (自己定义的数据类型)
 * @param   flags:读写标志位
 * @retval  无*/
esp_err_t i2c_transfer(i2c_obj_t *self, uint16_t addr, size_t n, i2c_buf_t *bufs, unsigned int  flags)
{
    int data_len = 0;
    esp_err_t ret = ESP_FAIL;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    /*根据器件通信时序区决定flags参数，进而选中如下代码不同的执行情况*/
    if ( flags & I2C_FLAG_WRITE )               /*写操作*/
    {
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd,addr<<1,ACK_CHECK_EN);
        i2c_master_write(cmd,bufs->buf,bufs->len,ACK_CHECK_EN);
        data_len += bufs->len;
        --n;                                    /*已经执行了一次*/
        ++bufs;
    }

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd,addr << 1 | (flags & I2C_FLAG_READ),ACK_CHECK_EN);                                     /*先发送写CMD*/

    for (; n--; ++bufs ) {
        if ( flags & I2C_FLAG_READ ) {
            i2c_master_read( cmd, bufs->buf, bufs->len, n == 0 ? I2C_MASTER_LAST_NACK : I2C_MASTER_ACK );        /*读命令CMD*/
        } else{
            if ( bufs->len != 0 ){
                i2c_master_write(cmd,bufs->buf,bufs->len,ACK_CHECK_EN);
            }
        }
        data_len += bufs->len;
    }

    if ( flags & I2C_FLAG_STOP )
    {
        i2c_master_stop(cmd);
    }

    ret = i2c_master_cmd_begin(self->port,cmd,100*(1 + data_len) / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    return ret;
}