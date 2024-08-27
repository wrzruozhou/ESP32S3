//
// Created by Administrator on 2024/8/26.
//

#ifndef WRZ_SB_RTC_H
#define WRZ_SB_RTC_H

#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sys/time.h"

/*时间结构体, 包括年月日周时份秒等信息*/
typedef struct
{
    uint8_t hour;          /*时*/
    uint8_t min;            /*分*/
    uint8_t sec;            /*秒*/
    /*公历年月日周*/
    uint16_t year;          /*年*/
    uint8_t month;          /*月*/
    uint8_t date;           /*日*/
    uint8_t week;           /*周*/
} _calendar_obj;

extern _calendar_obj calendar;          /* 时间结构体 */

void rtc_set_time(int year,int mon,int mday,int hour,int min,int sec);
void rtc_get_time(void);
uint8_t rtc_get_week(uint16_t year, uint8_t month, uint8_t day);
#endif   //WRZ_SB_RTC_H
