/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void *esp_h264_rc_hd_t;  /*<! Rate control(RC) handle */

/**
 * @brief  Create a new RC handle
 *
 * @param  qp_max     Maxinum of quantization parameter(QP). The range is [0, 51]. The smaller QP, the better video quality and the lower compression rate
 * @param  qp_min     Mininum of quantization parameter(QP). The range is [0, 51]. The smaller QP, the better video quality and the lower compression rate
 * @param  bitrate    Target bitrate desired
 * @param  fps        Frame per second
 * @param  mb_width   The width of picture in macroblocks
 * @param  mb_height  The height of picture in macroblock
 *
 * @return
 *       - >0    Rate control handle
 *       - NULL  Allcated memory failed.
 */
esp_h264_rc_hd_t esp_h264_enc_hw_rc_new(uint8_t qp_max, uint8_t qp_min, uint32_t bitrate, uint8_t fps, uint8_t mb_width, uint8_t mb_height);

/**
 * @brief  Set quantization parameter(QP)
 *
 * @param  rc_hd   Rate control handle
 * @param  qp_max  Maxinum of quantization parameter(QP). The range is [0, 51]. The smaller QP, the better video quality and the lower compression rate
 * @param  qp_min  Mininum of quantization parameter(QP). The range is [0, 51]. The smaller QP, the better video quality and the lower compression rate
 */
void esp_h264_enc_hw_rc_set_qp(esp_h264_rc_hd_t rc_hd, uint8_t qp_max, uint8_t qp_min);

/**
 * @brief  Set bitrate and frames per second(FPS)
 *
 * @param  rc_hd    Rate control handle
 * @param  bitrate  Target bitrate desired
 * @param  fps      Frame per second
 */
void esp_h264_enc_hw_rc_set_bt_fps(esp_h264_rc_hd_t rc_hd, uint32_t bitrate, uint8_t fps);

/**
 * @brief  RC start
 *
 * @param  rc_hd      Rate control handle
 * @param  is_iframe  Intra frame or not, true: it is intra frame, false: it isn't intra frame
 * @param  rate       rate = target bit / predicted bit
 * @param  pred_mad   Predicted mean absolute difference (MAD)
 * @param  qp         Quantization parameter(QP)
 */
void esp_h264_rc_start(esp_h264_rc_hd_t rc_hd, bool is_iframe, uint32_t *rate, uint32_t *pred_mad, uint8_t *qp);

/**
 * @brief  RC end
 *
 * @param  rc_hd           Rate control handle
 * @param  total_enc_bits  Total bit of encoded data
 * @param  frame_qp_sum    The quantization parameter(QP) sum of frame
 * @param  frame_mad_sum   The mean absolute difference (MAD) sum of frame
 */
void esp_h264_rc_end(esp_h264_rc_hd_t rc_hd, uint32_t total_enc_bits, uint32_t frame_qp_sum, uint32_t frame_mad_sum);

/**
 * @brief  Delete RC handle
 *
 * @param  rc_hd  Rate control handle
 */
void esp_h264_enc_hw_rc_del(esp_h264_rc_hd_t rc_hd);
#ifdef __cplusplus
}
#endif
