//
// Created by Administrator on 2024/10/24.
//

#ifndef WRZ_WIFI_SMARTCONFIG_H
#define WRZ_WIFI_SMARTCONFIG_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "nvs_flash.h"
#include "RGBLCD.h"
#include <esp_netif.h>
#include <esp_event.h>
#include <esp_wifi.h>
#include <esp_wifi_default.h>
#include <esp_smartconfig.h>
#include <esp_log.h>
#include <string.h>

void wifi_smartconfig_sta(void );

#endif   // WRZ_WIFI_SMARTCONFIG_H
