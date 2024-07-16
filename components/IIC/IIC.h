//
// Created by Administrator on 2024/6/29.
//

#ifndef __IIC_H
#define __IIC_H

#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_err.h"

typedef struct _i2c_obj_t{
    i2c_port_t port;
    gpio_num_t scl;
    gpio_num_t sda;
    esp_err_t  init_flag;
}i2c_obj_t;

typedef struct _i2c_buf_t{
    size_t len;
    uint8_t *buf;
}i2c_buf_t;

/*读写标志位置*/
#define I2C_FLAG_READ   (0x01)
#define I2C_FLAG_STOP   (0x02)
#define I2C_FLAG_WRITE  (0x04)

/*读写标志位*/
#define IIC0_SDA_GPIO_PIN   GPIO_NUM_41
#define IIC0_SCL_GPIO_PIN   GPIO_NUM_42
#define IIC1_SDA_GPIO_PIN   GPIO_NUM_5
#define IIC1_SCL_GPIO_PIN   GPIO_NUM_4
#define IIC_FREQ            400000
#define I2C_MASTER_TX_BUF_DISABLE   0
#define I2C_MASTER_RX_BUF_DISABLE   0
#define ACK_CHECK_EN                0x1

i2c_obj_t iic_init(uint8_t iic_port);
esp_err_t i2c_transfer(i2c_obj_t *self, uint16_t addr, size_t n, i2c_buf_t *bufs, unsigned int  flags);

#endif //WRZ_IIC_H
