/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_h264_enc_dual.h"
#include "esp_h264_enc_param_hw.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Structure for configuring the dual h264 encoder
 */
typedef struct {
    esp_h264_enc_cfg_t cfg0;  /*!< Configuration for the first stream */
    esp_h264_enc_cfg_t cfg1;  /*!< Configuration for the second stream */
} esp_h264_enc_cfg_dual_hw_t;

/**
 * @brief  This function is used to create a new instance of the `esp_h264_enc_dual_hw_t` data structure,
 *         which represents a dual-streams H.264 encoder in hardware
 *
 * @note  The dual streams must have the same GOP (Group of Pictures) setting
 *        The encoder's GOP is calculated as the average of the first stream's GOP and the second stream's GOP
 *        The GOP value will be updated in the intra frame
 *
 * @param[in]   cfg      It is a pointer to the `esp_h264_enc_cfg_dual_hw_t` structure, which contains the configuration settings for the encoder
 * @param[out]  out_enc  It is a double pointer to the `esp_h264_enc_dual_t` structure, which will store the created encoder instance
 *
 * @return
 *       - ESP_H264_ERR_OK   Succeeded
 *       - ESP_H264_ERR_ARG  Invalid arguments passed
 *       - ESP_H264_ERR_MEM  Insufficient memory, the `*out_enc` will be set NULL
 */
esp_h264_err_t esp_h264_enc_dual_hw_new(const esp_h264_enc_cfg_dual_hw_t *cfg, esp_h264_enc_dual_handle_t *out_enc);

/**
 * @brief  This function returns a pointer to the first stream hardware-encoded parameter structure associated with the given `esp_h264_enc_dual_t` encoder
 *
 * @param[in]   enc        The encoder instance that is from `esp_h264_enc_dual_hw_new`
 * @param[out]  out_param  The parameter set handle
 *
 * @return
 *       - ESP_H264_ERR_OK   Succeeded
 *       - ESP_H264_ERR_ARG  Invalid arguments passed
 */
esp_h264_err_t esp_h264_enc_dual_hw_get_param_hd0(esp_h264_enc_dual_handle_t enc, esp_h264_enc_param_hw_handle_t *out_param);

/**
 * @brief  This function returns a pointer to the second stream hardware-encoded parameter structure associated with the given `esp_h264_enc_dual_t` encoder
 *
 * @param[in]   enc        The encoder instance that is from `esp_h264_enc_dual_hw_new`
 * @param[out]  out_param  The parameter set handle
 *
 * @return
 *       - ESP_H264_ERR_OK   Succeeded
 *       - ESP_H264_ERR_ARG  Invalid arguments passed
 */
esp_h264_err_t esp_h264_enc_dual_hw_get_param_hd1(esp_h264_enc_dual_handle_t enc, esp_h264_enc_param_hw_handle_t *out_param);

#ifdef __cplusplus
}
#endif
