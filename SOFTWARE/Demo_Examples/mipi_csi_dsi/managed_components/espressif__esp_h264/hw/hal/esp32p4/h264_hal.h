/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "h264_ll.h"

#ifdef __cplusplus
extern "C" {
#endif

#define H264_INTR_DB_TMP_READY  (0x1 << 0)
#define H264_INTR_REC_READY     (0x1 << 1)
#define H264_INTR_FRAME_DONE    (0x1 << 2)
#define H264_INTR_2MB_LINE_DONE (0x1 << 3)
#define H264_INTR_MASK          (0xf)
#define H264_SUP_MAX_CHANNEL    (2)
#define H264_SCORE_LUMA         (6)
#define H264_SCORE_CHROMA       (7)
#define ESP_H264_QP_MAX         (51)

typedef h264_ctrl_regs_t *esp_h264_set_dev_t;  /*<! The hardware h264 device configure handle */

/**
 * @brief  Context of the HAL
 */
typedef struct h264_hal_context {
    h264_dev_t *dev;  /*<! H.264 peripheral address */
} h264_hal_context_t;

/**
 * @brief  H.264 stream configure information
 *         MB : macroblocks
 *         QP : quantization parameter
 *
 */
typedef struct h264_hal_channel_cfg {
    uint8_t score_chroma;         /*<! Chroma MB decimate score */
    uint8_t score_luma;           /*<! Luma MB decimate score */
    uint8_t intra_luma_offset;    /*<! Intra luma MB decimate score offset */
    uint8_t intra_chroma_offset;  /*<! Intra chroma MB decimate score offset */
    uint8_t inter_luma_offset;    /*<! Predicted luma MB decimate score offset */
    uint8_t inter_chroma_offset;  /*<! Predicted chroma MB decimate score offset */
    uint8_t cost_thres;           /*<! Intra frame cost threshold */
    uint8_t chroma_delta_qp;      /*<! Chroma QP offset based on luma QP*/
    uint8_t chroma_dc_delta_qp;   /*<! Chroma DC QP offset based on Chroma QP*/
    uint8_t db_bypass;            /*<! Disable de-blocking filter */
    uint8_t roi_mode;             /*<! Range of interesting (ROI) mode */
    uint8_t roi_none_roi_qp;      /*<! Delta QP of none ROI region */
    uint8_t rc_ena;               /*<! Rate control enable */
    uint8_t mvm_ena;              /*<! Motion vector enable */
} h264_hal_channel_cfg_t;

/**
 * @brief  H.264 configure information
 */
typedef struct h264_hal_context_cfg {
    bool                   gop_mode_en;                   /*<! Ture: GOP mode enable
                                                               False: frame mode enable
                                                               The dual stream encoding must use frame mode. */
    uint8_t                gop;                           /*<! Group of picture(GOP) */
    bool                   dual_stream_en;                /*<! Dual stream enable */
    h264_hal_channel_cfg_t cfg_ch[H264_SUP_MAX_CHANNEL];  /*<! H.264 stream configure information */
} h264_hal_context_cfg_t;

/**
 * @brief  Initalization the H.264
 *
 * @param  hal  Context of the HAL layer
 * @param  cfg  H.264 configuration
 */
void h264_hal_init(h264_hal_context_t *hal, h264_hal_context_cfg_t *cfg);

/**
 * @brief  Get stream configure handle
 *
 * @param  hal  Context of the HAL layer
 *
 * @return
 *       - Stream 0 configure handle
 */
esp_h264_set_dev_t h264_hal_get_param_dev0(h264_hal_context_t *hal);

/**
 * @brief  Get stream configure handle
 *
 * @param  hal  Context of the HAL layer
 *
 * @return
 *       - Stream 1 configure handle
 */
esp_h264_set_dev_t h264_hal_get_param_dev1(h264_hal_context_t *hal);

/**
 * @brief  Get H.264 interrput status
 *
 * @param  hal  Context of the HAL layer
 *
 * @return
 *       - H.264  interrput status
 */
uint32_t h264_hal_get_intr_status(h264_hal_context_t *hal);

/**
 * @brief  Clear H.264 interrput status
 *
 * @param  hal   Context of the HAL layer
 * @param  intr  H.264 interrput status
 */
void h264_hal_clear_intr_status(h264_hal_context_t *hal, uint32_t intr);

/**
 * @brief  Enable H.264 interrput
 *
 * @param  hal   Context of the HAL layer
 * @param  intr  H.264 interrput status
 */
void h264_hal_ena_intr(h264_hal_context_t *hal, uint32_t intr);

/**
 * @brief  Start H.264 encoder
 *
 * @param  hal  Context of the HAL layer
 */
void h264_hal_set_start(h264_hal_context_t *hal);

/**
 * @brief  Reset H.264 encoder
 *
 * @param  hal  Context of the HAL layer
 */
void h264_hal_reset(h264_hal_context_t *hal);

/**
 * @brief  Set rate control quantization parameter (QP)
 *
 * @param  device  Stream configure handle
 * @param  ena     true: enable rate control, false: disable rate control
 * @param  qp_min  Mininum of quantization parameter(QP), range in [0, 51]
 * @param  qp_max  Maxinum of quantization parameter(QP), range in [0, 51]
 */
void h264_hal_set_rc_qp(esp_h264_set_dev_t device, bool ena, uint8_t qp_min, uint8_t qp_max);

/**
 * @brief  Get rate control quantization parameter (QP)
 *
 * @param  device  Stream configure handle
 * @param  qp_min  Mininum of quantization parameter(QP), range in [0, 51]
 * @param  qp_max  Maxinum of quantization parameter(QP), range in [0, 51]
 */
void h264_hal_get_rc_qp(esp_h264_set_dev_t device, uint8_t *qp_min, uint8_t *qp_max);

/**
 * @brief  Set rate control parameter
 *
 * @param  device    Stream configure handle
 * @param  rate      The rate is between target bit and predicted bit
 * @param  pred_mad  Predicted mean absolute difference (MAD)
 *
 */
void h264_hal_set_rc_rate_pred(esp_h264_set_dev_t device, uint32_t rate, uint32_t pred_mad);

/**
 * @brief  Get rate control parameter
 *
 * @param  hal       Context of the HAL layer
 * @param  enc_bits  Encoder data bit length
 * @param  mad       Mean absolute difference (MAD) for the frame
 * @param  qp_sum    The sum of Quantizer parameter (QP)
 */
void h264_hal_get_rc_bits_mad_qpsum(h264_hal_context_t *hal, uint32_t *enc_bits, uint32_t *mad, uint32_t *qp_sum);

/**
 * @brief  Set motion vector(MV) mode
 *
 * @param  device   Stream configure handle
 * @param  mv_mode  Motion vector(MV) mode
 *                  0: The 16 * 16 macro block MV data collection
 *                  1: If sub-macro-block is exist, the MV data is minimum of sub-macro-block MV data.
 *                     Otherwise it's result of 16 * 16 macro block MV data.
 *                  2: If sub-macro-block is exist, the MV data is maximum of sub-macro-block MV data.
 *                     Otherwise it's result of 16 * 16 macro block MV data.
 * @param  mv_fmt   Motion vector(MV) format
 *                  0: Output all MV data expect zero
 *                  1: Output horizontal or vertical direction absolute of MV data gather than or equal 4
 */
void h264_hal_set_mv_mode(esp_h264_set_dev_t device, int8_t mv_mode, uint8_t mv_fmt);

/**
 * @brief  Get motion vector(MV) mode
 *
 * @param  device   Stream configure handle
 * @param  mv_mode  Motion vector(MV) mode
 *                  -1: It is disable MV
 *                   0: The 16 * 16 macro block MV data collection
 *                   1: If sub-macro-block is exist, the MV data is minimum of sub-macro-block MV data.
 *                      Otherwise it's result of 16 * 16 macro block MV data.
 *                   2: If sub-macro-block is exist, the MV data is maximum of sub-macro-block MV data.
 *                      Otherwise it's result of 16 * 16 macro block MV data.
 * @param  mv_fmt   Motion vector(MV) format
 *                  0: Output all MV data expect zero
 *                  1: Output horizontal or vertical direction absolute of MV data gather than or equal 4
 */
void h264_hal_get_mv_mode(esp_h264_set_dev_t device, int8_t *mv_mode, uint8_t *mv_fmt);

/**
 * @brief  Get motion vector(MV) buffer actual length
 *
 * @param  device  Stream configure handle
 * @return
 */
uint32_t h264_hal_get_mvm_data_len(esp_h264_set_dev_t device);

/**
 * @brief  Set group of picture(GOP)
 *
 * @param  hal          Context of the HAL layer
 * @param  gop          Group of picture, range in [1 , 255]
 * @param  gop_mode_en  GOP mode enable, true: enable false: disable
 */
void h264_hal_set_gop(h264_hal_context_t *hal, uint8_t gop, bool gop_mode_en);

/**
 * @brief  Start move. DMA will move data to memory
 *
 * @param  hal  Context of the HAL layer
 */
void h264_hal_dma_move_start(h264_hal_context_t *hal);

/**
 * @brief  Get encoder data length in byte
 *
 * @param  hal  Context of the HAL layer
 *
 * @return
 *       - Encoder data length
 */
uint32_t h264_hal_get_coded_len(h264_hal_context_t *hal);

/**
 * @brief  Set width and height of picture in macroblocks
 *
 * @param  device     Stream configure handle
 * @param  mb_width   The width of picture in macroblocks
 * @param  mb_height  The height of picture in macroblock
 */
void h264_hal_set_mbres(esp_h264_set_dev_t device, uint8_t mb_width, uint8_t mb_height);

/**
 * @brief  Get width and height of picture in macroblocks
 *
 * @param  device     Stream configure handle
 * @param  mb_width   The width of picture in macroblocks
 * @param  mb_height  The height of picture in macroblock
 */
void h264_hal_get_mbres(esp_h264_set_dev_t device, uint8_t *mb_width, uint8_t *mb_height);

/**
 * @brief  Set quantizer parameter(QP)
 *
 * @param  device  Stream configure handle
 * @param  qp      Quantizer parameter(QP)
 */
void h264_hal_set_qp(esp_h264_set_dev_t device, uint8_t qp);

/**
 * @brief  Get quantizer parameter(QP)
 *
 * @param  device  Stream configure handle
 * @param  qp      Quantizer parameter(QP)
 */
void h264_hal_get_qp(esp_h264_set_dev_t device, uint8_t *qp);

/**
 * @brief  Set slice header
 *         If the header last byte is less than 8 bits,round it up to one byte.
 *         If the header is less than 3 bytes, write zero for the rest bits.
 *
 * @param  hal            Context of the HAL layer
 * @param  header         Slice header
 * @param  slice_bit_len  `header` actual bit length
 */
void h264_hal_set_slice_header(h264_hal_context_t *hal, uint32_t header[3], uint32_t slice_bit_len);

/**
 * @brief  Set range of interesting (ROI) mode
 *
 * @param  device             Stream configure handle
 * @param  roi_mode           Range of interesting mode. -1: ROI disbale
 * @param  none_roi_delta_qp  Delta quantization parameter(QP) of none ROI region
 *
 */
void h264_hal_set_roi_mode(esp_h264_set_dev_t device, int8_t roi_mode, int8_t none_roi_delta_qp);

/**
 * @brief  Get range of interesting (ROI) mode
 *
 * @param  device             Stream configure handle
 * @param  roi_mode           Range of interesting mode
 * @param  none_roi_delta_qp  Delta quantization parameter(QP) of none ROI region
 *
 * @return
 */
bool h264_hal_get_roi_mode(esp_h264_set_dev_t device, uint8_t *roi_mode, int8_t *none_roi_delta_qp);

/**
 * @brief  Set range of interesting (ROI) region
 *
 * @param  device  Stream configure handle
 * @param  ena     True: enable the region. false: disable the region
 * @param  x       Start position in horizontal direction. units macroblock size
 * @param  y       Start position in vertical direction. units macroblock size
 * @param  len_x   ROI region length in horizontal direction. units macroblock size
 * @param  len_y   ROI region length in vertical direction. units macroblock size
 * @param  qp      Fixed quantization parameter(QP) or delta QP. range in [-51, 51]
 *                 - ----------------------------------------------------------------------------
 *                            roi_mode                     ROI QP             NONE_ROI_QP
 *                 - -----------------------------------------------------------------------------
 *                            0                            qp + slice QP      slice QP + none_roi_delta_qp
 *                 - -----------------------------------------------------------------------------
 *                            1                            qp                 slice QP + none_roi_delta_qp
 *                 - -----------------------------------------------------------------------------
 * @param  reg_idx  The index of ROI region. It's must less than 8.
 */
void h264_hal_set_roi_reg(esp_h264_set_dev_t device, bool ena, uint8_t x, uint8_t y, uint8_t len_x, uint8_t len_y, int8_t qp, uint8_t reg_idx);

/**
 * @brief  Get range of interesting (ROI) region
 *
 * @param  device  Stream configure handle
 * @param  x       Start position in horizontal direction. units macroblock size
 * @param  y       Start position in vertical direction. units macroblock size
 * @param  len_x   ROI region length in horizontal direction. units macroblock size
 * @param  len_y   ROI region length in vertical direction. units macroblock size
 * @param  qp      Fixed quantization parameter(QP) or delta QP. range in [-51, 51]
 */
void h264_hal_get_roi_reg(esp_h264_set_dev_t device, uint8_t *x, uint8_t *y, uint8_t *xlen, uint8_t *ylen, int8_t *qp, uint8_t reg_idx);

/**
 * @brief  Get bs error
 *
 * @param  hal  Context of the HAL layer
 *
 * @return
 *       - True   Overflow happened
 *       - False  Encoder normal
 */
bool h264_hal_get_bs_bit_overflow(h264_hal_context_t *hal);

#ifdef __cplusplus
}
#endif
