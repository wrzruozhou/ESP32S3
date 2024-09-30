//
// Created by Administrator on 2024/9/29.
//

#ifndef WRZ_REMOTE_H
#define WRZ_REMOTE_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "freertos/queue.h"
#include "driver/rmt_rx.h"
#include "renc.h"
#include "esp_log.h"

/**引脚定义*/
#define RMT_IN_GPIO_PIN GPIO_NUM_2
#define RMT_RESOULUTION_HZ  1000000
#define RMT_RESOLUTION_HZ  1000000
#define RMT_NEC_DECODE_MARGIN   200                      /* 判断NEC时序时长的容差值，小于（值+此值），大于（值-此值）为正确 */

/**NEC 协议时序时间，协议头9.5ms 4.5ms 逻辑0两个电平时长，逻辑1两个电平时长，重复码两个电平时长*/
#define NEC_LEADING_CODE_DURATION_0 9000                /**起始码*/
#define NEC_LEADING_CODE_DURATION_1 4500
#define NEC_PAYLOAD_ZERO_DURATION_0 560                 /**低电平逻辑码*/
#define NEC_PAYLOAD_ZERO_DURATION_1 560
#define NEC_PAYLOAD_ONE_DURATION_0  560                 /**高电平逻辑码*/
#define NEC_PAYLOAD_ONE_DURATION_1  1690
#define NEC_REPEAT_CODE_DURATION_0  9000                 /**连发码*/
#define NEC_REPEAT_CODE_DURATION_1  2250

void remote_init(void );
void  remote_scan(rmt_symbol_word_t *rmt_nec_symbols, size_t  symbol_num);
bool RMT_Rx_Done_Callback(rmt_channel_handle_t rx_chan, const rmt_rx_done_event_data_t *edata, void *user_ctx);

#endif   // WRZ_REMOTE_H
