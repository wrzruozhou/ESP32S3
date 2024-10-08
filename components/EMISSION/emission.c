//
// Created by Administrator on 2024/9/30.
//

#include "emission.h"

/**保存NEC解码地址的和命令字节*/
uint16_t s_nec_code_address;
uint16_t s_nec_code_command;

QueueHandle_t receive_queue;
uint8_t tbuf[40];


/**
 * @brief   初始化RMT
 * @param   无
 * @retval  无
 * */
void emission_init(void )
{
    uint8_t t = 0;

    /**配置接收通道*/
    rmt_rx_channel_config_t rx_channel_cfg = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = RMT_RESOLUTION_HZ,
        .mem_block_symbols = 64,
        .gpio_num = RMT_RX_PIN,
    };
    rmt_channel_handle_t rx_channel = NULL;
    ESP_ERROR_CHECK(rmt_new_rx_channel(&rx_channel_cfg, &rx_channel));

    /**创建消息队列,接收红外编码*/
    QueueHandle_t  receive_queue = xQueueCreate(1, sizeof( rmt_rx_done_event_data_t ));
    assert(receive_queue);

    /**注册红外回调函数*/
    rmt_rx_event_callbacks_t cbs = {
        .on_recv_done = RMT_Rx_Done_Callback,
    };
    ESP_ERROR_CHECK( rmt_rx_register_event_callbacks(rx_channel, &cbs ,receive_queue));

    /**一下时间要求基于NEC协议*/
    rmt_receive_config_t receive_config = {
        .signal_range_min_ns = 1250,
        .signal_range_max_ns = 12000000,
    };

    /**配置发送通道*/
    rmt_tx_channel_config_t tx_channel_cfg = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = RMT_RESOLUTION_HZ,
        .mem_block_symbols = 64,
        .trans_queue_depth = 4,                             /**可以在后台挂起的事务数量*/
        .gpio_num = RMT_TX_PIN
    };
    rmt_channel_handle_t tx_channel = NULL;
    ESP_ERROR_CHECK( rmt_new_tx_channel(&tx_channel_cfg, &tx_channel));

    /**配置载波与占空比*/
    rmt_carrier_config_t carrier_cfg = {
        .frequency_hz = 38000,
        .duty_cycle = 0.33,
    };
    ESP_ERROR_CHECK( rmt_apply_carrier(tx_channel, &carrier_cfg));

    /**不会在循环重发送NEC帧*/
    rmt_transmit_config_t transmit_config = {
        .loop_count = 0,
    };

    /**配置编码器*/
    ir_nec_encoder_config_t nec_encoder_cfg = {
        .resolution = RMT_RESOLUTION_HZ
    };
    rmt_encoder_handle_t nec_encoder = NULL;
    ESP_ERROR_CHECK( rmt_new_ir_nec_encoder(&nec_encoder_cfg,&nec_encoder));

    /**使能发送，接收通道*/
    ESP_ERROR_CHECK( rmt_enable(tx_channel));
    ESP_ERROR_CHECK( rmt_enable(rx_channel));

    /**保存接收到的RMT符号*/
    rmt_symbol_word_t raw_symbols[64];
    rmt_rx_done_event_data_t rx_data;
    ESP_ERROR_CHECK(rmt_receive(rx_channel, raw_symbols,sizeof(raw_symbols), &receive_config));

    while ( 1 )
    {
        if ( xQueueReceive(receive_queue, &rx_data, pdMS_TO_TICKS(1000)) ==  pdPASS)
        {
            example_parse_nec_frame(rx_data.received_symbols,rx_data.num_symbols);
            ESP_ERROR_CHECK( rmt_receive(rx_channel,raw_symbols, sizeof(raw_symbols),&receive_config));
        }
        else
        {
            t++;
            if ( t == 0 )
            {
                t = 1;
            }
            const ir_nec_scan_code_t scan_code = {
                .command = t,
            };
            sprintf((char*)tbuf, "%d", scan_code.command);
            printf("TX KEYVAL = %d\n", scan_code.command);
            ESP_ERROR_CHECK(rmt_transmit(tx_channel, nec_encoder, &scan_code, sizeof(scan_code), &transmit_config));    /* 通过RMT发送信道传输数据 */
        }
    }
}




/**
 * @brief       判断数据时序长度是否在NEC时序时长容差范围内 正负 RMT_NEC_DECODE_MARGIN 的值以内
 * @param       无
 * @retval      无
 */
inline bool nec_check_in_range(uint32_t signal_duration, uint32_t spec_duration)
{
    return (signal_duration < (spec_duration + RMT_NEC_DECODE_MARGIN)) &&
           (signal_duration > (spec_duration - RMT_NEC_DECODE_MARGIN));
}

/**
 * @brief       对比数据时序长度判断是否为逻辑0
 * @param       无
 * @retval      无
 */
bool nec_parse_logic0(rmt_symbol_word_t *rmt_nec_symbols)
{
    return nec_check_in_range(rmt_nec_symbols->duration0, NEC_PAYLOAD_ZERO_DURATION_0) &&
           nec_check_in_range(rmt_nec_symbols->duration1, NEC_PAYLOAD_ZERO_DURATION_1);
}

/**
 * @brief       对比数据时序长度判断是否为逻辑1
 * @param       无
 * @retval      无
 */
bool nec_parse_logic1(rmt_symbol_word_t *rmt_nec_symbols)
{
    return nec_check_in_range(rmt_nec_symbols->duration0, NEC_PAYLOAD_ONE_DURATION_0) &&
           nec_check_in_range(rmt_nec_symbols->duration1, NEC_PAYLOAD_ONE_DURATION_1);
}

/**
 * @brief       检查数据帧是否为重复按键：一直按住同一个键
 * @param       无
 * @retval      无
 */
bool nec_parse_frame_repeat(rmt_symbol_word_t* rmt_nec_symbols)
{
    return nec_check_in_range(rmt_nec_symbols->duration0, NEC_REPEAT_CODE_DURATION_0) &&
           nec_check_in_range(rmt_nec_symbols->duration1, NEC_REPEAT_CODE_DURATION_1);
}

/**
 * @brief       将RMT接收结果解码出NEC地址和命令
 * @param       无
 * @retval      无
 */
bool nec_parse_frame(rmt_symbol_word_t* rmt_nec_symbols)
{
    rmt_symbol_word_t* cur = rmt_nec_symbols;
    uint16_t address = 0;
    uint16_t command = 0;

    bool valid_leading_code = nec_check_in_range(cur->duration0, NEC_LEADING_CODE_DURATION_0) &&
                              nec_check_in_range(cur->duration1, NEC_LEADING_CODE_DURATION_1);

    if (!valid_leading_code)
    {
        return false;
    }

    cur++;

    for (int i = 0; i < 16; i++)
    {
        if (nec_parse_logic1(cur))
        {
            address |= 1 << i;
        }
        else if (nec_parse_logic0(cur))
        {
            address &= ~(1 << i);
        }
        else
        {
            return false;
        }
        cur++;
    }

    for (int i = 0; i < 16; i++)
    {
        if (nec_parse_logic1(cur))
        {
            command |= 1 << i;
        }
        else if (nec_parse_logic0(cur))
        {
            command &= ~(1 << i);
        }
        else
        {
            return false;
        }
        cur++;
    }

    /* 保存数据地址和命令，用于判断重复按键 */
    s_nec_code_address = address;
    s_nec_code_command = command;

    return true;
}

/**
 * @brief   根据NEC编码解析红外协议并打印指令结果
 * @param   无
 * @param   无
 * */
void example_parse_nec_frame(rmt_symbol_word_t* rmt_nec_symbols, size_t symbol_num)
{
    switch (symbol_num) /* 解码RMT接收数据 */
    {
        case 34:        /* 正常NEC数据帧 */
        {
            if (nec_parse_frame(rmt_nec_symbols))
            {
                sprintf((char*)tbuf, "%d", s_nec_code_command);
                printf("RX KEYCNT = %d\n", s_nec_code_command);
            }
            break;
        }

        case 2:         /* 重复NEC数据帧 */
        {
            if (nec_parse_frame_repeat(rmt_nec_symbols))
            {
                printf("RX KEYCNT = %d, repeat\n", s_nec_code_command);
            }
            break;
        }

        default:        /* 未知NEC数据帧 */
        {
            printf("Unknown NEC frame\r\n\r\n");
            break;
        }
    }
}

/**
 * @brief   RMT数据接收完成回调函数
 * @param   无
 * @param   无
 * */
bool RMT_Rx_Done_Callback(rmt_channel_handle_t channel, const rmt_rx_done_event_data_t *edata,void* user_data)
{
    BaseType_t high_task_wakeup = pdFALSE;
    QueueHandle_t receive_queue = (QueueHandle_t)user_data;

    xQueueSendFromISR(receive_queue, edata, &high_task_wakeup);         /* 将收到的RMT数据通过消息队列发送到解析任务 */
    return  high_task_wakeup == pdTRUE;
}


