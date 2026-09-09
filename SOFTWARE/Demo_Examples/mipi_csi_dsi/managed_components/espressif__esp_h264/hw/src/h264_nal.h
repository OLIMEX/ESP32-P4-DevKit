/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Configure sequence parameter set (SPS)
 *
 * @param  buffer  The address is to save network abstract layer(NAL) header + SPS
 * @param  len     The length of `buffer`
 * @param  height  Height of picture
 * @param  width   Width of picture
 * @param  fps     Frame per second
 *
 * @return
 *       - The bit length of SPS
 */
uint16_t esp_h264_enc_set_sps(uint8_t *buffer, uint16_t len, uint16_t height, uint16_t width, uint8_t fps);

/**
 * @brief  Configure picture parameter set (PPS)
 *
 * @param  buffer  The address is to save network abstract layer(NAL) header + PPS
 * @param  len     The length of `buffer`
 * @param  qp      Quantizer parameter(QP)
 * @param  db_ena  The de-blocking filter is enable or not. true  enable false  disable
 *
 * @return
 *       - The bit length of PPS
 */
uint16_t esp_h264_enc_set_pps(uint8_t *buffer, uint16_t len, uint8_t qp, bool db_ena);

/**
 * @brief  Set the slice header
 *
 * @param  buffer     The address is to save network abstract layer(NAL) header  + slice header
 * @param  len        The length of `buffer`
 * @param  is_iframe  Intra frame or not, true: it is intra frame, false: it isn't intra frame.
 * @param  frame_num  The number of frame
 * @param  qp_delta   The delta quantization parameter(QP) is between currently QP and initial QP
 * @param  db_ena     The de-blocking filter is enable or not, true: enable, false: disable
 *
 * @return
 *       - The bit length of slice header
 */
uint16_t esp_h264_enc_hw_set_slice(uint8_t *buffer, uint32_t len, bool is_iframe, uint32_t frame_num, int8_t qp_delta, bool db_ena);

#ifdef __cplusplus
}
#endif
