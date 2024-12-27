//
// Created by Administrator on 2024/9/11.
//

#ifndef WRZ_AP3216C_H
#define WRZ_AP3216C_H

#include "XL9555.h"
#include "driver/gpio.h"

/**AP3216C信息*/
typedef struct _ap3216c_value_t
{
    uint16_t ir;
    uint16_t als;
    uint16_t ps;
}ap3216c_value_t;

//相关参数定义
#define AP3216C_INT xl9555_pin_read( AP_INT_IO )
#define AP3216C_ADDR    0x1E

uint8_t ap3216c_config(void );
void ap3216c_init(i2c_obj_t self);
void ap3216c_read_data(uint16_t *ir, uint16_t *ps, uint16_t *als);
void ap3216c_Int(void );

#endif   // WRZ_AP3216C_H
