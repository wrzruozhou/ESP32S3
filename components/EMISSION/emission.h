//
// Created by Administrator on 2024/9/30.
//

#ifndef WRZ_EMISSION_H
#define WRZ_EMISSION_H

#include "stdio.h"
#include "stdlib.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_idf_version.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/rmt_tx.h"
#include "driver/rmt_rx.h"
#include "renc.h"

/**引脚定义*/
#define RMT_RX_PIN  GPIO_NUM_2
#define RMT_TX_PIN  GPIO_NUM_8
#define RMT_RESOLUTION_HZ   1000000
#define RMT_NEC_DECODE_MARGIN 200   /* 判断NEC时序时长的容差值，小于（值+此值），大于（值-此值）为正确 */

/* NEC 协议时序时间，协议头9.5ms 4.5ms 逻辑0两个电平时长，逻辑1两个电平时长，重复码两个电平时长 */
#define NEC_LEADING_CODE_DURATION_0 9000
#define NEC_LEADING_CODE_DURATION_1 4500
#define NEC_PAYLOAD_ZERO_DURATION_0 560
#define NEC_PAYLOAD_ZERO_DURATION_1 560
#define NEC_PAYLOAD_ONE_DURATION_0  560
#define NEC_PAYLOAD_ONE_DURATION_1  1690
#define NEC_REPEAT_CODE_DURATION_0  9000
#define NEC_REPEAT_CODE_DURATION_1  2250

void emission_init(void );
void example_parse_nec_frame(rmt_symbol_word_t* rmt_nec_symbols, size_t symbol_num);
bool RMT_Rx_Done_Callback(rmt_channel_handle_t channel, const rmt_rx_done_event_data_t *edata,void* user_data);


#endif   // WRZ_EMISSION_H
