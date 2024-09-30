//
// Created by Administrator on 2024/9/29.
//

#include "remote.h"

const static  char *TAG = "REMOTE_RECEIVE TEST";

/**保存NEC解码的地址和命令*/
uint16_t s_nec_code_address;
uint16_t s_nec_code_command;

QueueHandle_t receive_queue;
rmt_channel_handle_t rx_channel;
rmt_symbol_word_t raw_symbols[64];
rmt_receive_config_t receive_config;
rmt_rx_done_event_data_t rx_data;

/**
 * @brief   初始化RMT
 * @param   无
 * @retval   无
 * */
void remote_init(void )
{
    /**配置接收通道*/
    ESP_LOGI(TAG,"Create RMT RX channel");

    rmt_rx_channel_config_t  rx_channel_cfg;
    rx_channel_cfg.clk_src = RMT_CLK_SRC_DEFAULT;
    rx_channel_cfg.resolution_hz = RMT_RESOULUTION_HZ;
    rx_channel_cfg.mem_block_symbols = 64;
    rx_channel_cfg.gpio_num = RMT_IN_GPIO_PIN;

    rx_channel = NULL;
    ESP_ERROR_CHECK( rmt_new_rx_channel(&rx_channel_cfg, &rx_channel));

    /**配置RMT接收通道回调函数*/
    ESP_LOGI(TAG,"register RX done callback");
    receive_queue = xQueueCreate(1,sizeof(rmt_rx_done_event_data_t));
    assert(receive_queue);
    rmt_rx_event_callbacks_t cbs = {
        .on_recv_done = RMT_Rx_Done_Callback,
    };
    ESP_ERROR_CHECK( rmt_rx_register_event_callbacks(rx_channel, &cbs, receive_queue));

    /** 以下时间要求基于NEC协议 */
    rmt_receive_config_t receive_config = {
        .signal_range_min_ns = 1250,                        //NEC 信号的最短持续时间为 560 µs，由于 1250 ns < 560 µs，有效信号不会视为噪声
        .signal_range_max_ns = 12000000,                    //NEC 信号的最长持续时间为 9000 µs，由于 12000000 ns > 9000 µs，接收不会提前停止
    };

    /**创建一个NEC协议的RMT编码器*/
    ESP_LOGI(TAG,"install IR NEC encoder");
    ir_nec_encoder_config_t nec_encoder_cfg = {
        .resolution = RMT_RESOULUTION_HZ,
    };

    rmt_encoder_handle_t nec_encoder = NULL;
    ESP_ERROR_CHECK( rmt_new_ir_nec_encoder(&nec_encoder_cfg, &nec_encoder));

    /**配置RMT接收通道*/
    ESP_LOGI(TAG,"开启 RMT 接收通道");
    ESP_ERROR_CHECK( rmt_enable(rx_channel));
    ESP_ERROR_CHECK( rmt_receive(rx_channel, raw_symbols, sizeof( raw_symbols ), &receive_config));

    while ( 1 )
    {
        /**以下时间要求基于NEC协议*/
        if ( xQueueReceive(receive_queue, &rx_data, pdMS_TO_TICKS(1000)) == pdPASS )
        {
            remote_scan(rx_data.received_symbols, rx_data.num_symbols);

            ESP_ERROR_CHECK( rmt_receive(rx_channel,raw_symbols, sizeof(raw_symbols),&receive_config));
        }
    }
}

/**
 * @brief   判断数据时序长度是否在NEC时序时长容差范围内 正负 REMOTE_NEC_DECODE_MARGIN的值以内
 * @param   无
 * @retval  无
 * */
inline bool rmt_nec_check_range(uint32_t signal_duration, uint32_t spec_duration)
{
    return (signal_duration < (spec_duration + RMT_NEC_DECODE_MARGIN)) &&
           (signal_duration > (spec_duration - RMT_NEC_DECODE_MARGIN));
}

/**
 * @brief   对比数据时序长度判断是否为逻辑0
 * @param   无
 * @retval  无
 * */
bool rmt_nec_logic0(rmt_symbol_word_t *rmt_nec_symbols)
{
    return rmt_nec_check_range(rmt_nec_symbols->duration0,NEC_PAYLOAD_ZERO_DURATION_0) &&
           rmt_nec_check_range(rmt_nec_symbols->duration1, NEC_PAYLOAD_ZERO_DURATION_1);
}

/**
 * @brief   对比数据时序长度判断是否为逻辑1
 * @param   无
 * @retval  无
 * */
bool rmt_nec_logic1(rmt_symbol_word_t *rmt_nec_symbols)
{
    return rmt_nec_check_range(rmt_nec_symbols->duration0, NEC_PAYLOAD_ONE_DURATION_0) &&
           rmt_nec_check_range(rmt_nec_symbols->duration1, NEC_PAYLOAD_ONE_DURATION_1);
}

/**
 * @brief   检查数据帧是否为重复按键:一直按住同一个键
 * @param   无
 * @retval  无
 * */
bool rmt_nec_parse_frame_repeat(rmt_symbol_word_t *rmt_nec_symbols)
{
    return rmt_nec_check_range(rmt_nec_symbols->duration0, NEC_REPEAT_CODE_DURATION_0) &&
           rmt_nec_check_range(rmt_nec_symbols->duration1, NEC_REPEAT_CODE_DURATION_1);
}

/**
 * @brief   将RMT接收结果解码出NEC地址和命令
 * @param   无
 * @param   无
 * */
bool rmt_nec_parse_frame(rmt_symbol_word_t *rmt_nec_symbols)
{
    rmt_symbol_word_t *cur = rmt_nec_symbols;
    uint16_t  address = 0;
    uint16_t  command = 0;
    bool valid_leading_code = rmt_nec_check_range(cur->duration0, NEC_LEADING_CODE_DURATION_0) &&
                              rmt_nec_check_range(cur->duration1, NEC_LEADING_CODE_DURATION_1);
    if ( !valid_leading_code )
    {
        return false;
    }
    cur++;
    for ( int i = 0; i < 16; ++i )
    {
        if ( rmt_nec_logic1(cur) )
        {
            address |= 1 << i;
        }
        else if( rmt_nec_logic0(cur) )
        {
            address &= ~(1 << i);
        }
        else
        {
            return false;
        }
        cur++;
    }

    for ( int i = 0; i < 16; ++i )
    {
        if ( rmt_nec_logic1(cur) )
        {
            command |= 1 << i;
        }
        else if( rmt_nec_logic0(cur))
        {
            command &= ~(1 << i);
        }
        else
        {
            return  false;
        }
        cur++;
    }
    /**保存数据地址和命令，用于判断重复按键*/
    s_nec_code_address = address;
    s_nec_code_command = command;
    return true;
}

/**
 * @brief   根据NEC编码器解析红外协议并打印指令结果
 * @param   无
 * @param   无
 * */
void  remote_scan(rmt_symbol_word_t *rmt_nec_symbols, size_t  symbol_num)
{
    uint8_t rmt_data = 0;
    uint8_t  tbuf[40];
    char *str = 0;

    switch ( symbol_num )                               /**解码RMT接收数据*/
    {
        case 34: /**正常数据帧*/
        {
            if ( rmt_nec_parse_frame( rmt_nec_symbols ) )
            {
                rmt_data = ( s_nec_code_command >> 8 );
                switch ( rmt_data )
                {
                    case 0xBA:
                    {
                        str = "POWER";
                        break;
                    }

                    case 0xB9:
                    {
                        str = "UP";
                        break;
                    }

                    case 0xB8:
                    {
                        str = "ALIENTEK";
                        break;
                    }

                    case 0xBB:
                    {
                        str = "BACK";
                        break;
                    }

                    case 0xBF:
                    {
                        str = "PLAY/PAUSE";
                        break;
                    }

                    case 0xBC:
                    {
                        str = "FORWARD";
                        break;
                    }

                    case 0xF8:
                    {
                        str = "vol-";
                        break;
                    }

                    case 0xEA:
                    {
                        str = "DOWN";
                        break;
                    }

                    case 0xF6:
                    {
                        str = "VOL+";
                        break;
                    }

                    case 0xE9:
                    {
                        str = "1";
                        break;
                    }

                    case 0xE6:
                    {
                        str = "2";
                        break;
                    }

                    case 0xF2:
                    {
                        str = "3";
                        break;
                    }

                    case 0xF3:
                    {
                        str = "4";
                        break;
                    }

                    case 0xE7:
                    {
                        str = "5";
                        break;
                    }

                    case 0xA1:
                    {
                        str = "6";
                        break;
                    }

                    case 0xF7:
                    {
                        str = "7";
                        break;
                    }

                    case 0xE3:
                    {
                        str = "8";
                        break;
                    }

                    case 0xA5:
                    {
                        str = "9";
                        break;
                    }

                    case 0xBD:
                    {
                        str = "0";
                        break;
                    }

                    case 0xB5:
                    {
                        str = "DLETE";
                        break;
                    }
                }
                sprintf( ( char * )tbuf, "%d", rmt_data );
                printf( "KEYVAL = %d, Command=%X\n", rmt_data, s_nec_code_command );
            }
            break;
        }
        case 2: /**重复数据*/
        {
            if ( rmt_nec_parse_frame_repeat( rmt_nec_symbols ) )
            {
                printf( "KEYVAL = %d, Command = %04X, repeat\n", rmt_data, s_nec_code_command );
            }
            break;
        }
        default:
        {
            printf( "Unknown NEC frame\r\n\r\n" );
            break;
        }
    }
}

/**
 * @brief   RMT数据接收完成回调函数
 * @param   无
 * @retval  无
 * */
bool RMT_Rx_Done_Callback(rmt_channel_handle_t rx_chan, const rmt_rx_done_event_data_t *edata, void *user_ctx)
{
    BaseType_t high_task_wakeup = pdFALSE;

    QueueHandle_t receive_queue = (QueueHandle_t)user_ctx;
    // 将接收到的 RMT 符号发送到解析任务的消息队列中
    xQueueSendFromISR(receive_queue, edata, &high_task_wakeup);     /**将收到的RMT数据通过消息队列发送到解析任务*/
    // 返回是否唤醒了任何任务
    return high_task_wakeup == pdTRUE;
}