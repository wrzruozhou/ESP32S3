//
// Created by Administrator on 2024/11/29.
//

#ifndef WRZ_GT1151_H
#define WRZ_GT1151_H

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "XL9555.h"
#include "touch.h"

#define GT9XXX_INT_GPIO_PIN GPIO_NUM_40
#define GT9XXX_IIC_PORT     I2C_NUM_1
#define GT9XXX_IIC_SDA      GPIO_NUM_39
#define GT9XXX_IIC_SCL      GPIO_NUM_38
#define GT9XXX_IIC_FREQ      400000      //400khz
#define GT9XXX_INT          gpio_get_level(GT9XXX_INT_GPIO_PIN)     /*中断引脚*/

/*RGB_BL*/
#define CT_RST(x)   do{ \
    (x)?                  \
        xl9555_pin_write(CT_RST_IO, 1): \
        xl9555_pin_write(CT_RST_IO, 0); \
    }while(0);

/*IIC读写命令*/
#define GT9XXX_CMD_WR           0x28    /*写命令*/
#define GT9XXX_CMD_RD           0x29    /*读命令*/

/*GT9xx部分寄存器定义*/
#define GT9XXX_CTRL_REG     0x8040      /*GT9XXX控制寄存器*/
#define GT9XXX_CFGS_REG     0x8047      /*GT9XXX配置起始地址寄存器*/
#define GT9XXX_CHECK_REG    0x80ff      /*GT9XXX校验和寄存器*/
#define GT9XXX_PID_REG      0x8140      /*GT9XXX产品ID寄存器*/

#define GT9XXX_GSTID_REG    0x814E      /*GT9XXX当前检测到的触摸情况*/
#define GT9XXX_TP1_REG      0x8150      /*第一个触摸点的数据地址*/
#define GT9XXX_TP2_REG      0x8158      /*第二个触摸点的数据地址*/
#define GT9XXX_TP3_REG      0x8160      /*第三个触摸点的数据地址*/
#define GT9XXX_TP4_REG      0x8168      /*第四个触摸点的数据地址*/
#define GT9XXX_TP5_REG      0x8170      /*第五个触摸点的数据地址*/
#define GT9XXX_TP6_REG      0x8178      /*第六个触摸点的数据地址*/
#define GT9XXX_TP7_REG      0x8180      /*第七个触摸点的数据地址*/
#define GT9XXX_TP8_REG      0x8188      /*第八个触摸点的数据地址*/
#define GT9XXX_TP9_REG      0x8190      /*第九个触摸点的数据地址*/
#define GT9XXX_TP10_REG     0x8198      /*第十个触摸点的数据地址*/

uint8_t gt9xxx_init(void );
uint8_t gt9xxx_scan(uint8_t mode);
void load_draw_dialog(void);
void lcd_draw_bline(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t size, uint16_t color);

#endif   // WRZ_GT1151_H
