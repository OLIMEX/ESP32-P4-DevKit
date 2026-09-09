/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_h264_enc_single_sw.h"
#include "esp_h264_enc_single.h"

/**
 * @brief Single hardware oneencoding. And this case add set/get base paramter function test in one thread.
 *        The set/get paramter function can be call in one thread or asynchronous thread.
 *        It is simple test case. And do the follow opertion.
 *        step 1: Create one encoder handle by call `esp_h264_enc_sw_new`.
 *        step 2: Open the encoder by call `esp_h264_enc_open`.
 *        step 3: `esp_h264_enc_process` is called through the loop until the encoding ends.
 *        step 4: Close the encoder handle by call `esp_h264_enc_close`.
 *        step 5: Delete the encoder handle by call `esp_h264_enc_del`.
 *
 * @param  cfg  THe configuration of single software encoder
 *
 * @return
 *       - ESP_H264_ERR_OK           Succeeded
 *       - ESP_H264_ERR_TIMEOUT      Timeout
 *       - ESP_H264_ERR_ARG          Invalid arguments passed
 *       - ESP_H264_ERR_MEM          Insufficient memory
 *       - ESP_H264_ERR_FAIL         Failed
 *       - ESP_H264_ERR_UNSUPPORTED  Process feature is not supported by the encoder.
 */
esp_h264_err_t single_sw_enc_thread_test(esp_h264_enc_cfg_sw_t cfg);
