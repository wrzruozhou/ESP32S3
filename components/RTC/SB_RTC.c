//
// Created by Administrator on 2024/8/26.
//

#include "SB_RTC.h"

_calendar_obj calendar;          /* 时间结构体 */

/**
 * @brief   RTC时间设置
 * @param   year    :   年
 * @param   mon     :   月
 * @param   mday    :   日
 * @param   hour    :   时
 * @param   min     :   分
 * @param   sec     :   秒
 * @retval  无
 * */
void rtc_set_time(int year,int mon,int mday,int hour,int min,int sec)
{
    struct tm datetime;

    /*设置时间*/
    datetime.tm_year = year-1900;
    datetime.tm_mon = mon - 1;
    datetime.tm_mday = mday;
    datetime.tm_hour = hour;
    datetime.tm_min = min;
    datetime.tm_sec = sec - 1;
    datetime.tm_isdst = -1;

    /*获取1970.1.1以来的总秒数*/
    time_t  second = mktime(&datetime);
    struct timeval timeval = {.tv_sec = second,.tv_usec = 0};
    /*设置当前时间*/
    settimeofday(&timeval,NULL);
}

/**
 * @brief   获取当前的时间
 * @param   无
 * @param   无*/
void rtc_get_time(void)
{
    struct tm *datetime;
    time_t second;
    /*返回自(1970.1.1 00:00:00 UTC)经过的时间(秒)*/
    time(&second);

    datetime = localtime(&second);

    calendar.hour = datetime->tm_hour;
    calendar.min = datetime->tm_min;
    calendar.sec = datetime->tm_sec;

    /*公历年月日*/
    calendar.year = datetime->tm_year + 1900;
    calendar.month = datetime->tm_mon + 1;
    calendar.date = datetime->tm_mday;

    calendar.week = rtc_get_week(calendar.year, calendar.month, calendar.date);
}

/**
 * @brief               将年月日时分秒转化成秒钟数
 * @note                输入公历日期得到星期(起始时间为:   公园0年3月1日开始，
 *                      输入往后的任何日期，都可以获取正确的日期
 *                      使用 基姆拉尔森计算公式 计算，原理说明见此帖
 *                      https://www.cnblogs.com/fengbohello/p/3264300.html)
 * @param               syear   :   年份
 * @param               smon    :   月份
 * @param               sday    :   日期
 * @retval              0,星期天；1~6星期1~星期六
 * */
uint8_t rtc_get_week(uint16_t year, uint8_t month, uint8_t day)
{
    uint8_t  week = 0;
    if ( month < 3 )
    {
        month += 12;
        --year;
    }
    week = (day + 1 + 2 * month + 3 * (month + 1) / 5 + year + (year >> 2) - year / 100 + year / 400) % 7;
    return week;
}