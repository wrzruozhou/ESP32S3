//
// Created by Administrator on 2024/9/14.
//

//#ifndef WRZ_RENC_H
//#define WRZ_RENC_H
#pragma once

#include "stdint.h"
#include "driver/rmt_encoder.h"

#ifdef __cplusplus
extern "C" {
#endif
/**红外指令组成结构体*/
typedef struct {
    uint16_t address;
    uint16_t command;
}ir_nec_scan_code_t;

/**红外NEC编码器配置*/
typedef struct {
    uint32_t resolution;        /** 编码器分辨率,单位Hz */
}ir_nec_encoder_config_t;

esp_err_t rmt_new_ir_nec_encoder(const ir_nec_encoder_config_t *config, rmt_encoder_handle_t *ret_encoder);

#ifdef __cplusplus
}
#endif   // WRZ_RENC_H
