//
// Created by Administrator on 2024/9/9.
//

#include "ADCM.h"

#if USE_IDF_V5
/**
 * @brief   初始化ADC
 * @param   无
 * @retval  无
 * */
void adc_init(void )
{
    adc_digi_configuration_t adc1_config;           /**adc1 初始化句柄*/
    adc_digi_pattern_config_t adc1_pattern;         /**adc1 配置句柄*/
    adc1_pattern.channel = ADC_ADCX_CHY;            /**配置ADC通道*/
    adc1_pattern.unit = ADC_UNIT_1;                 /**配置使用ADC1*/
    adc1_pattern.bit_width = ADC_BITWIDTH_12;       /**配置ADC位宽*/
    adc1_pattern.atten = ADC_ATTEN_DB_11;           /**配置ADC衰减程度*/
    adc1_config.adc_pattern = &adc1_pattern;

    adc_digi_controller_configure(&adc1_config);    /**配置ADC1*/
}


/**
 * @brief   获取ADC转换且进行均值滤波的结果
 * @param   ch  :通道号
 * @param   times:获取次数
 * @retval
 * */
 uint32_t adc_get_result_average(uint32_t ch, uint8_t times)
 {
     uint32_t temp_val = 0;
     uint8_t t;
     for ( t = 0; t < times; ++t )
     {
         temp_val += adc1_get_raw(ch);
         vTaskDelay(5);
     }

     return temp_val/times;         /**返回平均值*/
 }
#else
 /**
 * @brief   初始化ADC
 * @note    V5版本初始化ADC
 * @param   无
 * @retval  无
 * */
 adc_oneshot_unit_handle_t adc1_ret_unit;
 void adc_init_v5(void )
 {
     adc_oneshot_unit_init_cfg_t adc1_init_config;

     adc1_init_config.unit_id = ADC_UNIT_1;
     adc1_init_config.ulp_mode = ADC_ULP_MODE_DISABLE;
     adc1_init_config.clk_src = 0;                                               /*使用默认的时钟源*/
     adc_oneshot_new_unit(&adc1_init_config, &adc1_ret_unit);

     /**配置ADC通道*/
     adc_oneshot_chan_cfg_t adc1_config = {
         .bitwidth = ADC_BITWIDTH_12,
         .atten = ADC_ATTEN_DB_11
     };
     adc_oneshot_config_channel(adc1_ret_unit, ADC_CHANNEL_7,&adc1_config);
 }

 /**
 * @brief   获取ADC转换且进行均值滤波的结果
  * @note   采用IDFV5版本写的代码
 * @param   ch  :通道号
 * @param   times:获取次数
 * @retval
 * */
 uint32_t adc_get_result_average_V5(uint32_t ch, uint8_t times)
 {
     uint32_t temp_val = 0;
     uint32_t sum = 0;
     uint8_t t;
     for ( t = 0; t < times; ++t )
     {
         adc_oneshot_read(adc1_ret_unit,ADC_CHANNEL_7,(int *)&temp_val);
         sum += temp_val;
         vTaskDelay(5);
     }
     return sum / times;
 }
#endif