//
// Created by Administrator on 2024/6/29.
//

#ifndef __XL9555_H
#define __XL9555_H

/*定义引脚中断INT*/
#define XL9555_INT_IO GPIO_NUM_40
#define XL9555_INT  gpio_get_level(XL9555_INT_IO)

/*XL9555命令宏*/
#define XL9555_INPUT_PORT0_REG 0
#define XL9555_INPUT_PORT1_REG 1
#define XL9555_OUTPUT_PORT0_REG 2
#define XL9555_OUTPUT_PORT1_REG 3
#define XL9555_INVERSION_PORT0_REG 4
#define XL9555_INVERSION_PORT1_REG 5
#define XL9555_CONFIG_PORT0_REG 6
#define XL9555_CONFIG_PORT1_REG 7

/*XL9555地址*/
#define XL9555_ADDR 0x20

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

/*按键被按下的标志*/
#define KEY0_PRES 0
#define KEY1_PRES 1
#define KEY2_PRES 2
#define KEY3_PRES 3

#endif //WRZ_XL9555_H
