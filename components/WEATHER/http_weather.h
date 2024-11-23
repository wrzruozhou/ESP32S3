//
// Created by Administrator on 2024/11/23.
//

#ifndef WRZ_HTTP_WEATHER_H
#define WRZ_HTTP_WEATHER_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <esp_http_client.h>
#include <esp_log.h>
#include "cJSON.h"
#include "cJSON_Utils.h"

#define MAX_HTTP_OUTPUT_BUFF 512

void http_test_task(void *pvParameters);

#endif   // WRZ_HTTP_WEATHER_H
