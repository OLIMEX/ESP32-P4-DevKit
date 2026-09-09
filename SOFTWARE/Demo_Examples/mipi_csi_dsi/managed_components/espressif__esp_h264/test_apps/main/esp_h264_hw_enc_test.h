/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#if CONFIG_IDF_TARGET_ESP32P4
#include "esp_h264_enc_single_hw.h"
#include "esp_h264_enc_single.h"
#include "esp_h264_enc_dual_hw.h"
#include "esp_h264_enc_dual.h"

/**
 * @brief Single hardware encoding. It is simple test case. And do the follow opertion.
 *        step 1: Create one encoder handle by call `esp_h264_enc_hw_new`.
 *        step 2: Open the encoder by call `esp_h264_enc_open`.
 *        step 3: `esp_h264_enc_process` is called through the loop until the encoding ends.
 *        step 4: Close the encoder handle by call `esp_h264_enc_close`.
 *        step 5: Delete the encoder handle by call `esp_h264_enc_del`.
 *
 * @param  cfg  THe configuration of single hardware encoder
 *
 * @return
 *       - ESP_H264_ERR_OK           Succeeded
 *       - ESP_H264_ERR_TIMEOUT      Timeout
 *       - ESP_H264_ERR_ARG          Invalid arguments passed
 *       - ESP_H264_ERR_MEM          Insufficient memory
 *       - ESP_H264_ERR_FAIL         Failed
 *       - ESP_H264_ERR_UNSUPPORTED  Process feature is not supported by the encoder.
 */
esp_h264_err_t single_hw_enc_process(esp_h264_enc_cfg_hw_t cfg);

/**
 * @brief Dual Hardware encoding. It is simple test case. And do the follow opertion.
 *        step 1: Create one encoder handle by call `esp_h264_enc_dual_hw_new`.
 *        step 2: Open the encoder by call `esp_h264_enc_dual_open`.
 *        step 3: `esp_h264_enc_dual_process` is called through the loop until the encoding ends.
 *        step 4: Close the encoder handle by call `esp_h264_enc_dual_close`.
 *        step 5: Delete the encoder handle by call `esp_h264_enc_dual_del`.
 *
 * @param  cfg  THe configuration of dual Hardware encoder
 *
 * @return
 *       - ESP_H264_ERR_OK           Succeeded
 *       - ESP_H264_ERR_TIMEOUT      Timeout
 *       - ESP_H264_ERR_ARG          Invalid arguments passed
 *       - ESP_H264_ERR_MEM          Insufficient memory
 *       - ESP_H264_ERR_FAIL         Failed
 *       - ESP_H264_ERR_UNSUPPORTED  Process feature is not supported by the encoder.
 */
esp_h264_err_t dual_hw_enc_process(esp_h264_enc_cfg_dual_hw_t cfg);

/**
 * @brief Single hardware oneencoding. And this case add set/get base paramter function test than `single_hw_enc_process` in one thread.
 *        The set/get paramter function can be call in one thread or asynchronous thread.
 *
 * @param  cfg  THe configuration of single hardware encoder
 *
 * @return
 *       - ESP_H264_ERR_OK           Succeeded
 *       - ESP_H264_ERR_TIMEOUT      Timeout
 *       - ESP_H264_ERR_ARG          Invalid arguments passed
 *       - ESP_H264_ERR_MEM          Insufficient memory
 *       - ESP_H264_ERR_FAIL         Failed
 *       - ESP_H264_ERR_UNSUPPORTED  Process feature is not supported by the encoder.
 */
esp_h264_err_t single_hw_enc_thread_test(esp_h264_enc_cfg_hw_t cfg);

/**
 * @brief Dual Hardware encoding. And this case add paramter set/get base function test than `dual_hw_enc_process` in one thread.
 *        The set/get paramter function can be call in one thread or asynchronous thread.
 *
 * @param  cfg  THe configuration of dual Hardware encoder
 *
 * @return
 *       - ESP_H264_ERR_OK           Succeeded
 *       - ESP_H264_ERR_TIMEOUT      Timeout
 *       - ESP_H264_ERR_ARG          Invalid arguments passed
 *       - ESP_H264_ERR_MEM          Insufficient memory
 *       - ESP_H264_ERR_FAIL         Failed
 *       - ESP_H264_ERR_UNSUPPORTED  Process feature is not supported by the encoder.
 */
esp_h264_err_t dual_hw_enc_thread_test(esp_h264_enc_cfg_dual_hw_t cfg);

/**
 * @brief Single hardware encoding. This case is for ROI configure test.
 *        The ROI component has two mode. One is fixed QP, the other is delta QP. It is for ROI.
 *        The one ROI QP can be changed by `none_roi_delta_qp` in `esp_h264_enc_roi_cfg_t`.
 *
 * @param  cfg  THe configuration of single hardware encoder
 *
 * @return
 *       - ESP_H264_ERR_OK           Succeeded
 *       - ESP_H264_ERR_TIMEOUT      Timeout
 *       - ESP_H264_ERR_ARG          Invalid arguments passed
 *       - ESP_H264_ERR_MEM          Insufficient memory
 *       - ESP_H264_ERR_FAIL         Failed
 *       - ESP_H264_ERR_UNSUPPORTED  Process feature is not supported by the encoder.
 */
esp_h264_err_t single_hw_enc_roi_cfg_test(esp_h264_enc_cfg_hw_t cfg);

/**
 * @brief Dual Hardware encoding. This case is for ROI configure test.
 *        The ROI component has two mode. One is fixed QP, the other is delta QP. It is for ROI.
 *        The one ROI QP can be changed by `none_roi_delta_qp` in `esp_h264_enc_roi_cfg_t`.
 *
 * @param  cfg  THe configuration of dual Hardware encoder
 *
 * @return
 *       - ESP_H264_ERR_OK           Succeeded
 *       - ESP_H264_ERR_TIMEOUT      Timeout
 *       - ESP_H264_ERR_ARG          Invalid arguments passed
 *       - ESP_H264_ERR_MEM          Insufficient memory
 *       - ESP_H264_ERR_FAIL         Failed
 *       - ESP_H264_ERR_UNSUPPORTED  Process feature is not supported by the encoder.
 */
esp_h264_err_t dual_hw_enc_roi_cfg_test(esp_h264_enc_cfg_dual_hw_t cfg);

/**
 * @brief Single hardware encoding. This case is for ROI region test.
 *        If you want use this function, please open the ROI component first.
 *        There are 8 region of ROI, which can be configured less than or equal to 8.Each region is a rectangular box.
 *        The coordinate width, height and QP and its starting position need to be configured.
 *
 * @param  cfg  THe configuration of single hardware encoder
 *
 * @return
 *       - ESP_H264_ERR_OK           Succeeded
 *       - ESP_H264_ERR_TIMEOUT      Timeout
 *       - ESP_H264_ERR_ARG          Invalid arguments passed
 *       - ESP_H264_ERR_MEM          Insufficient memory
 *       - ESP_H264_ERR_FAIL         Failed
 *       - ESP_H264_ERR_UNSUPPORTED  Process feature is not supported by the encoder.
 */
esp_h264_err_t single_hw_enc_roi_reg_test(esp_h264_enc_cfg_hw_t cfg);

/**
 * @brief Dual Hardware encoding. This case is for ROI region test.
 *        If you want use this function, please open the ROI component first.
 *        There are 8 region of ROI, which can be configured less than or equal to 8.Each region is a rectangular box.
 *        The coordinate width, height and QP and its starting position need to be configured.
 *
 * @param  cfg  THe configuration of dual Hardware encoder
 *
 * @return
 *       - ESP_H264_ERR_OK           Succeeded
 *       - ESP_H264_ERR_TIMEOUT      Timeout
 *       - ESP_H264_ERR_ARG          Invalid arguments passed
 *       - ESP_H264_ERR_MEM          Insufficient memory
 *       - ESP_H264_ERR_FAIL         Failed
 *       - ESP_H264_ERR_UNSUPPORTED  Process feature is not supported by the encoder.
 */
esp_h264_err_t dual_hw_enc_roi_reg_test(esp_h264_enc_cfg_dual_hw_t cfg);

/**
 * @brief Single hardware encoding. This case is for MV test.
 *        First call `esp_h264_enc_hw_cfg_mv` to configure MV.
 *        Before call `esp_h264_enc_process`, please configure MV packet by `esp_h264_enc_hw_set_mv_pkt`.
 *        After call `esp_h264_enc_process`, get MV length by `esp_h264_enc_hw_get_mv_data_len`
 *
 * @param  cfg  THe configuration of single hardware encoder
 *
 * @return
 *       - ESP_H264_ERR_OK           Succeeded
 *       - ESP_H264_ERR_TIMEOUT      Timeout
 *       - ESP_H264_ERR_ARG          Invalid arguments passed
 *       - ESP_H264_ERR_MEM          Insufficient memory
 *       - ESP_H264_ERR_FAIL         Failed
 *       - ESP_H264_ERR_UNSUPPORTED  Process feature is not supported by the encoder.
 */
esp_h264_err_t single_hw_enc_mv_pkt_test(esp_h264_enc_cfg_hw_t cfg);

/**
 * @brief Dual Hardware encoding. This case is for MV test.
 *        First call `esp_h264_enc_hw_cfg_mv` to configure MV.
 *        Before call `esp_h264_enc_dual_process`, please configure MV packet by `esp_h264_enc_hw_set_mv_pkt`.
 *        After call `esp_h264_enc_dual_process`, get MV length by `esp_h264_enc_hw_get_mv_data_len`
 *
 * @param  cfg  THe configuration of dual Hardware encoder
 *
 * @return
 *       - ESP_H264_ERR_OK           Succeeded
 *       - ESP_H264_ERR_TIMEOUT      Timeout
 *       - ESP_H264_ERR_ARG          Invalid arguments passed
 *       - ESP_H264_ERR_MEM          Insufficient memory
 *       - ESP_H264_ERR_FAIL         Failed
 *       - ESP_H264_ERR_UNSUPPORTED  Process feature is not supported by the encoder.
 */
esp_h264_err_t dual_hw_enc_mv_pkt_test(esp_h264_enc_cfg_dual_hw_t cfg);
#endif //CONFIG_IDF_TARGET_ESP32P4
