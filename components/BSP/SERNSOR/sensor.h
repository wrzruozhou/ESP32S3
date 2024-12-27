//
// Created by Administrator on 2024/10/9.
//

#ifndef WRZ_SENSOR_H
#define WRZ_SENSOR_H

#include "esp_err.h"
#include "stdio.h"
#include "string.h"
#include "inttypes.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/temperature_sensor.h"

/** 参数定义 */
#define SENSOR_RANGE_MIN 20
#define SENSOR_RANGE_MAX 50

void temperature_sensor_init(void );
float sensor_get_temperature(void );

#endif   // WRZ_SENSOR_H
