//
// Created by Administrator on 2024/10/14.
//

#ifndef WRZ_WIFI_SCAN_H
#define WRZ_WIFI_SCAN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include <nvs_flash.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_mac.h"

/* 存储12个WIFI名称 */
#define DEFAULT_SCAN_LIST_SIZE  12

void wifi_scan(void);

#endif   // WRZ_WIFI_SCAN_H
