//
// Created by Administrator on 2024/11/8.
//

#ifndef WRZ_TCP_MCLIENT_H
#define WRZ_TCP_MCLIENT_H

#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "sys/socket.h"
#include "RGBLCD.h"

#define LWIP_SEND_DATA 0x80         /*定义有收据发送*/
extern uint8_t g_lwip_send_flag;    /*数据发送标志位*/

void tcp_demo(void );


#endif   // WRZ_TCP_MCLIENT_H
