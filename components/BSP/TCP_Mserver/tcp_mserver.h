//
// Created by Administrator on 2024/11/9.
//

#ifndef WRZ_TCP_MSERVER_H
#define WRZ_TCP_MSERVER_H

#include "string.h"
#include "sys/socket.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "RGBLCD.h"

#define LWIP_SEND_DATA              0X80    /* 定义有数据发送 */
extern uint8_t g_lwip_send_flag_server;            /* 数据发送标志位 */

void server_demo(void );

#endif   // WRZ_TCP_MSERVER_H
