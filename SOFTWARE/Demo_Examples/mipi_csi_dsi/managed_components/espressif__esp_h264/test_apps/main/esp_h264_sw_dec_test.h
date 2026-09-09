/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_h264_dec_sw.h"

/**
 * @brief Single hardware oneencoding. It is simple test case. And do the follow opertion.
 *        step 1: Create one decoder handle by call `esp_h264_dec_sw_new`.
 *        step 2: Open the decoder by call `esp_h264_dec_open`.
 *        step 3: `esp_h264_dec_process` is called through the loop until the encoding ends.
 *        step 4: Close the decoder handle by call `esp_h264_dec_close`.
 *        step 5: Delete the decoder handle by call `esp_h264_dec_del`.
 *
 * @param  cfg        THe configuration of single software decoder
 * @param  inbuf      The input buffer
 * @param  inbuf_len  The inbuf length
 * @param  yuv        The output buffer
 *
 * @return
 *       - ESP_H264_ERR_OK           Succeeded
 *       - ESP_H264_ERR_TIMEOUT      Timeout
 *       - ESP_H264_ERR_ARG          Invalid arguments passed
 *       - ESP_H264_ERR_MEM          Insufficient memory
 *       - ESP_H264_ERR_FAIL         Failed
 *       - ESP_H264_ERR_UNSUPPORTED  Process feature is not supported by the decoder.
 */
esp_h264_err_t single_sw_dec_process(esp_h264_dec_cfg_sw_t cfg, uint8_t *inbuf, uint32_t inbuf_len, uint8_t *yuv);
