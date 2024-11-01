//
// Created by Administrator on 2024/10/16.
//

#ifndef WRZ_WIFI_STA_H
#define WRZ_WIFI_STA_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include <esp_log.h>

/**链接WIFI名称*/
#define DEFAULT_SSID    "muneo-办公室"
/**WIFI密码*/
#define DEFAULT_PWD     "muneo888"
/**事件标志*/
#define WIFI_CONNECTED_BIT  BIT0
#define WIFI_FAIL_BIT       BIT1


/**WIFI的默认配置*/
#define WIFICONFIG(){ \
    .sta = {          \
        .ssid = DEFAULT_SSID, \
        .password = DEFAULT_PWD, \
        .threshold.authmode = WIFI_AUTH_WPA2_PSK},        \
    }

void wifi_sta_init_O(void);

#endif   // WRZ_WIFI_STA_H
