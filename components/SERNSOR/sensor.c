//
// Created by Administrator on 2024/10/9.
//

#include "sensor.h"

esp_err_t rev_flag;
temperature_sensor_handle_t temp_handle = NULL;

/**
 * @brief   初始化内部温度传感器
 * @param   无
 * @param   无
 * */
void temperature_sensor_init(void )
{
    temperature_sensor_config_t temp_sensor;
    temp_sensor.range_min = SENSOR_RANGE_MIN;
    temp_sensor.range_max = SENSOR_RANGE_MAX;
    rev_flag |= temperature_sensor_install(&temp_sensor,&temp_handle);
    ESP_ERROR_CHECK(rev_flag);
}

/**
 * @brief   获取内部温度传感器的值
 * @param   无
 * @retval  返回内部温度值*/
float sensor_get_temperature(void )
{
    float temp;
    /**启用温度传感器*/
    rev_flag |= temperature_sensor_enable(temp_handle);

    /**获取传输的传感器的数据*/
    rev_flag |= temperature_sensor_get_celsius(temp_handle, &temp);

    /**温度传感器使用完毕后，禁用温度传感器，节约功耗*/
    rev_flag |= temperature_sensor_disable(temp_handle);

    ESP_ERROR_CHECK(rev_flag);
    return temp;
}
