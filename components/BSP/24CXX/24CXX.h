//
// Created by Administrator on 2024/9/6.
//

#ifndef WRZ_24CXX_H
#define WRZ_24CXX_H

#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "IIC.h"
#include "esp_log.h"

/* 24c02的设备地址为*/
#define AT_ADDR (0x50)

/* EEPROM大小为 */
#define  AT24C02 255

/*开发板使用的是24C02*/
#define EE_TYPE AT24C02

void at24cxx_init(i2c_obj_t self);
uint8_t at24cxx_read_one_byte(uint16_t addr);
void at24cxx_write_one_byte(uint16_t addr, uint8_t data);
uint8_t at24cxx_check(void );
void at24cxx_read(uint16_t addr,uint8_t *pbuf, uint16_t datalen);
void  at24cxx_write(uint16_t addr, uint8_t *pbuf, uint16_t datalen);

#endif   //WRZ_24CXX_H
