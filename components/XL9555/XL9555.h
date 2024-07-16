//
// Created by Administrator on 2024/6/29.
//

#ifndef __XL9555_H
#define __XL9555_H

#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "IIC.h"

/*定义引脚中断INT*/
#define XL9555_INT_IO GPIO_NUM_40
#define XL9555_INT  gpio_get_level(XL9555_INT_IO)

/*XL9555命令宏*/
#define XL9555_INPUT_PORT0_REG 0                                /* 输入寄存器0地址 */
#define XL9555_INPUT_PORT1_REG 1                                /* 输入寄存器1地址 */
#define XL9555_OUTPUT_PORT0_REG 2                               /* 输出寄存器0地址 */
#define XL9555_OUTPUT_PORT1_REG 3                               /* 输出寄存器1地址 */
#define XL9555_INVERSION_PORT0_REG 4                            /* 极性反转寄存器0地址 */
#define XL9555_INVERSION_PORT1_REG 5                            /* 极性反转寄存器1地址 */
#define XL9555_CONFIG_PORT0_REG 6                               /* 方向配置寄存器0地址 */
#define XL9555_CONFIG_PORT1_REG 7                               /* 方向配置寄存器1地址 */

/*XL9555地址*/
#define XL9555_ADDR 0x20                                        /* XL9555地址(左移了一位)-->请看手册（9.1. Device Address） */

/*GPIO端口的定义*/
#define AP_INT_IO   0x0001
#define QMA_INT_IO  0x0002
#define SPK_EN_IO   0x0004
#define BEEP_IO     0x0008
#define OV_PWDN_IO  0x0010
#define OV_RESET_IO 0x0020
#define GBC_LED_IO  0x0040
#define GBC_KEY_IO  0x0080

#define LCD_BL_IO   0x0100
#define CT_RST_IO   0x0200
#define SLCD_RST_IO 0x0400
#define SLCD_PWR_IO 0x0800
#define KEY3_IO     0x1000
#define KEY2_IO     0x2000
#define KEY1_IO     0x4000
#define KEY0_IO     0x8000

#define KEY0 xl9555_pin_read(KEY0_IO)
#define KEY1 xl9555_pin_read(KEY1_IO)
#define KEY2 xl9555_pin_read(KEY2_IO)
#define KEY3 xl9555_pin_read(KEY3_IO)

/*按键被按下的标志*/
#define KEY0_PRES 0                                                 /* KEY0按下 */
#define KEY1_PRES 1                                                 /* KEY1按下 */
#define KEY2_PRES 2                                                 /* KEY1按下 */
#define KEY3_PRES 3                                                 /* KEY1按下 */

/* 函数声明 */
void xl9555_init(i2c_obj_t self);                                   /* 初始化XL9555 */
int xl9555_pin_read(uint16_t pin);                                  /* 获取某个IO状态 */
uint16_t xl9555_pin_write(uint16_t pin, int val);                   /* 控制某个IO的电平 */
esp_err_t xl9555_read_byte(uint8_t* data, size_t len);              /* 读取XL9555的16位IO值 */
uint8_t xl9555_key_scan(uint8_t mode);                              /* 扫描按键值 */

#endif //WRZ_XL9555_H
