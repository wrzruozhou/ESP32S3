//
// Created by Administrator on 2024/10/18.
//

#ifndef WRZ_WIFI_AP_H
#define WRZ_WIFI_AP_H

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include <lwip/inet.h>

#define EXAMPLE_ESP_WIFI_SSID   "每小时自动支付1元" /**ESP创建的热点名称*/
#define EXAMPLE_ESP_WIFI_PASSWD ""        /**ESP创建热点的密码*/
#define EXMAPLE_ESP_MAXIMUM_RETRY   5           /**最大的连接个数*/

#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"

void wifi_init_softap(void );

#endif   // WRZ_WIFI_AP_H
