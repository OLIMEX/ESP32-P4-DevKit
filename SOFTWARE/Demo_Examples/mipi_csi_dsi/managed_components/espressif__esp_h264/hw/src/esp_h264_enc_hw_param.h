/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_h264_enc_param_hw.h"
#include "h264_hal.h"
#include "h264_dma_hal.h"
#include "h264_nal.h"
#include "h264_rc.h"
#include "esp_h264_mutex.h"
#include "esp_h264_cache.h"
#include "esp_h264_check.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_H264_MIN_WIDTH    (80)
#define ESP_H264_MIN_HEIGHT   (80)
#define ESP_H264_MAX_WIDTH    (1920)
#define ESP_H264_MAX_HEIGHT   (2032)
#define HW_DMA_BUF_ALIGN_SIZE (7)
#define H264_TIME_OUT         (1000)

/**
 * @brief  Configure information
 */
typedef struct esp_h264_set_cfg {
    esp_h264_set_dev_t device;   /*<! Device handle that configure encoders of different channels */
    uint16_t           width;    /*<! Width of picture */
    uint16_t           height;   /*<! Height of picture */
    uint8_t            qp_min;   /*<! The minimum quantization parameter(QP) */
    uint8_t            qp_max;   /*<! The maximum quantization parameter(QP) */
    uint8_t            fps;      /*<! Frames per second */
    uint32_t           bitrate;  /*<! Bit per second */
} esp_h264_enc_hw_param_cfg_t;

/**
 * @brief  Create a new parameter set handle
 *
 * @param[in]   cfg         It is a pointer to the `esp_h264_enc_hw_param_cfg_t` structure, which contains the configuration settings for the handle
 * @param[out]  out_handle  Hardware H.264 encoder parameter set handle
 *                          If return value isn't `ESP_H264_ERR_OK`, `handle` will be set NULL
 *
 * @return
 *       - ESP_H264_ERR_OK   Succeeded
 *       - ESP_H264_ERR_ARG  Arguments error
 *       - ESP_H264_ERR_MEM  Insufficient memory
 *
 */
esp_h264_err_t esp_h264_enc_hw_new_param(esp_h264_enc_hw_param_cfg_t *cfg, esp_h264_enc_param_hw_handle_t *out_handle);

/**
 * @brief  Delete parameter set handle
 *
 * @param[in]  handle  Hardware H.264 encoder parameter set handle
 *
 * @return
 *       - ESP_H264_ERR_OK  Succeeded
 */
esp_h264_err_t esp_h264_enc_hw_del_param(esp_h264_enc_param_hw_handle_t handle);

/**
 * @brief  Get mutual exclusion (MUTEX) parameter
 *
 * @param[in]   handle     Hardware H.264 encoder parameter set handle
 * @param[out]  out_mutex  Mutual exclusion
 *
 * @return
 *       - ESP_H264_ERR_OK  Succeeded
 */
esp_h264_err_t esp_h264_enc_hw_get_mutex(esp_h264_enc_param_hw_handle_t handle, esp_h264_mutex_t *out_mutex);

/**
 * @brief  Get the inital quantization parameter(QP)
 *
 * @param[in]   handle       Hardware H.264 encoder parameter set handle
 * @param[out]  out_qp_init  The inital quantization parameter(QP)
 *
 * @return
 *       - ESP_H264_ERR_OK  Succeeded
 */
esp_h264_err_t esp_h264_enc_hw_get_qp_init(esp_h264_enc_param_hw_handle_t handle, uint8_t *out_qp_init);

/**
 * @brief  Get sequence parameter set (SPS) and picture parameter set (PPS)
 *
 * @param[in]   handle           Hardware H.264 encoder parameter set handle
 * @param[out]  out_nal_buf      The buffer is saved sequence parameter set (SPS) and picture parameter set (PPS)
 * @param[out]  out_nal_bit_len  The bit length of `nal_buf`
 *
 * @return
 *       - ESP_H264_ERR_OK  Succeeded
 */
esp_h264_err_t esp_h264_enc_hw_get_nal(esp_h264_enc_param_hw_handle_t handle, uint8_t *out_nal_buf, uint16_t *out_nal_bit_len);

/**
 * @brief  Configure reference and de-blocking DMA descriptor
 *
 * @param[in]  handle     Hardware H.264 encoder parameter set handle
 * @param[in]  dma2d_hal  The 2DDMA handle
 *
 * @return
 *       - ESP_H264_ERR_OK  Succeeded
 */
esp_h264_err_t esp_h264_enc_hw_cfg_dma_db_ref(esp_h264_enc_param_hw_handle_t handle, h264_dma_hal_context_t *dma2d_hal);

/**
 * @brief  Configure un-encoder data and encoder data DMA descriptor
 *
 * @param[in]  handle      Hardware H.264 encoder parameter set handle
 * @param[in]  dma2d_hal   The 2DDMA handle
 * @param[in]  dsc_yuv     Un-encoder data DMA descriptor
 * @param[in]  buf_yuv     The buffer is to save un-encoder data
 * @param[in]  dsc_bs      Encoder data DMA descriptor
 * @param[in]  buf_bs      The buffer is to save encoder data
 * @param[in]  buf_bs_len  The length of `buf_bs`
 *
 * @return
 *       - ESP_H264_ERR_OK  Succeeded
 */
esp_h264_err_t esp_h264_enc_hw_cfg_dma_yuv_bs(esp_h264_enc_param_hw_handle_t handle, h264_dma_hal_context_t *dma2d_hal, h264_dma_desc_t *dsc_yuv, uint8_t *buf_yuv, h264_dma_desc_t *dsc_bs, uint8_t *buf_bs, uint32_t buf_bs_len);

/**
 * @brief  Configure de-blocking filter temporary parameter DMA descriptor
 *
 * @param[in]  handle     Hardware H.264 encoder parameter set handle
 * @param[in]  dma2d_hal  The 2DDMA handle
 * @param[in]  dsc        Descriptor
 * @param[in]  buf        The buffer is to save de-blocking filter temporary parameter
 *
 * @return
 *       - ESP_H264_ERR_OK  Succeeded
 */
esp_h264_err_t esp_h264_enc_hw_cfg_dma_dbtmp(esp_h264_enc_param_hw_handle_t handle, h264_dma_hal_context_t *dma2d_hal, h264_dma_desc_t *dsc[2], uint8_t *buf);

/**
 * @brief  Configure motion vector DMA descriptor
 *
 * @param[in]  handle     Hardware H.264 encoder parameter set handle
 * @param[in]  dma2d_hal  The 2DDMA handle
 *
 * @return
 *       - ESP_H264_ERR_OK  Succeeded
 */
esp_h264_err_t esp_h264_enc_hw_cfg_dma_mvm(esp_h264_enc_param_hw_handle_t handle, h264_dma_hal_context_t *dma2d_hal);

/**
 * @brief  Set quantizer parameter(QP)
 *
 * @param[in]  handle  Hardware H.264 encoder parameter set handle
 * @param[in]  qp      Quantizer parameter(QP)
 *
 * @return
 *       - ESP_H264_ERR_OK  Succeeded
 */
esp_h264_err_t esp_h264_enc_hw_set_qp(esp_h264_enc_param_hw_handle_t handle, uint8_t qp);

/**
 * @brief  Get Rate control handle
 *
 * @param[in]   handle     Hardware H.264 encoder parameter set handle
 * @param[out]  out_rc_hd  Rate control handle
 *
 * @return
 *       - ESP_H264_ERR_OK  Succeeded
 */
esp_h264_err_t esp_h264_enc_hw_get_rc_hd(esp_h264_enc_param_hw_handle_t handle, esp_h264_rc_hd_t *out_rc_hd);

/**
 * @brief  Set rate control parameter
 *
 * @param[in]  handle    Hardware H.264 encoder parameter set handle
 * @param[in]  rate      The rate is between target bit and predicted bit. The parameter is from `esp_h264_rc_start`.
 * @param[in]  pred_mad  Predicted mean absolute difference (MAD). The parameter is from `esp_h264_rc_start`.
 *
 * @return
 *       - ESP_H264_ERR_OK  Succeeded
 */
esp_h264_err_t esp_h264_enc_hw_set_rc_rate_pred(esp_h264_enc_param_hw_handle_t handle, int32_t rate, int32_t pred_mad);

/**
 * @brief  Get width and height of picture in macroblocks
 *
 * @param[in]   handle         Hardware H.264 encoder parameter set handle
 * @param[out]  out_mb_width   The width of picture in macroblocks
 * @param[out]  out_mb_height  The height of picture in macroblock
 *
 * @return
 *       - ESP_H264_ERR_OK  Succeeded
 */
esp_h264_err_t esp_h264_enc_hw_get_mbres(esp_h264_enc_param_hw_handle_t handle, uint8_t *out_mb_width, uint8_t *out_mb_height);

/**
 * @brief  Check resolution
 *
 * @param[in]  width   Width of picture
 * @param[in]  height  Height of picture
 *
 * @return
 *       - ESP_H264_ERR_OK    Succeeded
 *       - ESP_H264_ERR_FAIL  Failed
 */
esp_h264_err_t esp_h264_enc_hw_res_check(uint16_t width, uint16_t height);

/**
 * @brief  Get de-blocking filter temporary buffer size
 *
 * @param[in]  width  Width of picture
 *
 * @return
 *       - The size of de-blocking filter temporary buffer
 */
uint16_t esp_h264_enc_hw_max_db_tmp_buffer_size(uint16_t width);

/**
 * @brief  Make bit stream buffer aligned to 8 byte
 *
 * @param[in]  bs        The bit stream buffer address
 * @param[in]  bit_len   The bit size of `bs`
 * @param[in]  h264_hal  The hardware H.264 handle
 *
 * @return
 *       - The aligned `bs` address
 */
uint8_t *esp_h264_enc_hw_slice_header_align8(uint8_t *bs, int32_t bit_len, h264_hal_context_t *h264_hal);

#ifdef __cplusplus
}
#endif
