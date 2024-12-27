//
// Created by Administrator on 2024/9/9.
//

#ifndef WRZ_ADCM_H
#define WRZ_ADCM_H



#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
/**
 * @note    原子使用的API是V4的，我使用的API是V5的，注意，两个API不可同时编译
 * @param   0:使用V5 API
 *          1:使用V4 API
 * */
#define USE_IDF_V5 0

#if USE_IDF_V5
#include <driver/adc.h>
#define ADC_ADCX_CHY (ADC1_CHANNEL_7)

void adc_init(void );
uint32_t adc_get_result_average(uint32_t ch, uint8_t times);

#else
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_continuous.h"
#define ADC_ADCX_CHY (ADC_CHANNEL_7)

void adc_init_v5(void );
uint32_t adc_get_result_average_V5(uint32_t ch, uint8_t times);

#endif


#endif   //WRZ_ADCM_H
