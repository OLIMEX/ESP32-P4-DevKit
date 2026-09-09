/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <string.h>
#include "esp_h264_enc_param.h"
#include "codec_api.h"
#include "codec_app_def.h"
#include "codec_def.h"
#include "esp_h264_check.h"
#include "esp_h264_alloc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_H264_SW_MIN_WIDTH  (16)
#define ESP_H264_SW_MIN_HEIGHT (16)

/**
 * @brief  Configuration structure for software-based H.264 encoder parameters
 */
typedef struct esp_h264_set_cfg {
    ISVCEncoder *device; /*<! Device handle that configure encoders by `WelsCreateSVCEncoder`*/
} esp_h264_enc_sw_param_cfg_t;

/**
 * @brief  Create a new parameter set handle
 *
 * @param  cfg     It is a pointer to the `esp_h264_enc_hw_param_cfg_t` structure, which contains the configuration settings for the handle
 * @param  handle  Hardware H.264 encoder parameter set handle.
 *                 If return value isn't `ESP_H264_ERR_OK`, `handle` will be set NULL
 *
 * @return
 *       - ESP_H264_ERR_OK   Succeeded
 *       - ESP_H264_ERR_ARG  Invalid arguments passed
 *       - ESP_H264_ERR_MEM  Insufficient memory, the `*handle` will be set NULL
 *
 */
esp_h264_err_t esp_h264_enc_sw_new_param(esp_h264_enc_sw_param_cfg_t *cfg, esp_h264_enc_param_t **handle);

/**
 * @brief  Delete parameter set handle
 *
 * @param  handle  Hardware H.264 encoder parameter set handle
 *
 * @return
 *       - ESP_H264_ERR_OK  Succeeded
 */
esp_h264_err_t esp_h264_enc_sw_del_param(esp_h264_enc_param_t *handle);

#ifdef __cplusplus
}
#endif
