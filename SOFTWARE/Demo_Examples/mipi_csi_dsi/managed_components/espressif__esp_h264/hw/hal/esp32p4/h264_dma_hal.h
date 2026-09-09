/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "h264_dma_ll.h"

#ifdef __cplusplus
extern "C" {
#endif

#define H264_DMA_OUT_CONF0_EOF_EN     (1 << 1)
#define H264_DMA_OUT_CONF0_REORDER_EN (1 << 16)
#define H264_DMA_OUT_MAX_CH_NUM       (5)
#define H264_DMA_IN_MAX_CH_NUM        (6)
#define H264_DMA_BURST_SIZE           (4)
#define H264_DMA_END_ADDR             (~0)

/**
 * @brief  DMA configure information
 */
typedef struct h264_dma_hal_context_cfg {
    uint32_t exter_addr_start[2];                    /*<! Start extern memory address */
    uint32_t exter_addr_end[2];                      /*<! Stop extern memory address */
    uint32_t inter_addr_start[2];                    /*<! Start internal memory address */
    uint32_t inter_addr_end[2];                      /*<! Stop internal memory address */
    uint32_t out_ch_conf0[H264_DMA_OUT_MAX_CH_NUM];  /*<! Transmit(TX) channel configure */
    uint32_t in_ch_conf0[H264_DMA_IN_MAX_CH_NUM];    /*<! Recive(RX) channel configure */
    uint8_t  burst_size;                             /*<! Burst size for all channels */
} h264_hal_dma_context_cfg_t;

/**
 * @brief  Context of the HAL
 */
typedef struct h264_dam_hal_context {
    h264_dma_dev_t *dev;  /*<! DMA peripheral address */
} h264_dma_hal_context_t;

/**
 * @brief  Initalization the H.264 DMA master
 *
 * @param  hal  Context of the HAL layer
 * @param  cfg  DMA configuration
 */
void h264_dma_hal_init(h264_dma_hal_context_t *hal, h264_hal_dma_context_cfg_t *cfg);

/**
 * @brief  De-initalization the H.264 master
 *
 * @param  hal  Context of the HAL layer
 */
void h264_dma_hal_deinit(h264_dma_hal_context_t *hal);

/**
 * @brief  Get DMA interrput of the encoder data
 *
 * @param  hal  Context of the HAL layer
 *
 * @return
 *       - The interrupt
 */
uint32_t h264_dma_hal_get_bs_intr(h264_dma_hal_context_t *hal);

/**
 * @brief  Clear the DMA interrput
 *
 * @param  hal  Context of the HAL layer
 */
void h264_dma_hal_clear_intr(h264_dma_hal_context_t *hal);

/**
 * @brief  Start the un-encoder DMA channel in TX direction
 *
 * @param  hal  Context of the HAL layer
 */
void h264_dma_hal_start_yuv_dma(h264_dma_hal_context_t *hal);

/**
 * @brief  Start the refrence frame DMA channel in TX direction
 *
 * @param  hal  Context of the HAL layer
 */
void h264_dma_hal_start_ref_dma(h264_dma_hal_context_t *hal);

/**
 * @brief  Start the de-blocking filter temporary parameter DMA channel in TX direction
 *
 * @param  hal  Context of the HAL layer
 */
void h264_dma_hal_start_tx_dbtmp_dma(h264_dma_hal_context_t *hal);

/**
 * @brief  Start the de-blocking filter lines DMA channel in TX direction
 *
 * @param  hal  Context of the HAL layer
 */
void h264_dma_hal_start_tx_db12_4_dma(h264_dma_hal_context_t *hal);

/**
 * @brief  Start the de-blocking filter lines DMA channel in RX direction
 *
 * @param  hal  Context of the HAL layer
 */
void h264_dma_hal_start_rx_db12_4_dma(h264_dma_hal_context_t *hal);

/**
 * @brief  Start the de-blocking filter temporary parameter DMA channel in RX direction
 *
 * @param  hal  Context of the HAL layer
 */
void h264_dma_hal_start_rx_dbtmp_dma(h264_dma_hal_context_t *hal);

/**
 * @brief  Start the MVM DMA channel in RX direction
 *
 * @param  hal  Context of the HAL layer
 */
void h264_dma_hal_start_rx_mvm_dma(h264_dma_hal_context_t *hal);

/**
 * @brief  Start the encoded data DMA channel in RX direction
 *
 * @param  hal  Context of the HAL layer
 */
void h264_dma_hal_start_rx_bs_dma(h264_dma_hal_context_t *hal);

/**
 * @brief  Reset de-blocking fliter line DMA counter
 *
 * @param  hal  Context of the HAL layer
 */
void h264_dma_hal_reset_counter_db(h264_dma_hal_context_t *hal);

/**
 * @brief  Reset the de-blocking filter temporary parameter DMA counter
 *
 * @param  hal  Context of the HAL layer
 */
void h264_dma_hal_reset_counter_dbtmp(h264_dma_hal_context_t *hal);

/**
 * @brief  Start the refrence frame DMA channel DMA counter
 *
 * @param  hal  Context of the HAL layer
 */
void h264_dma_hal_reset_counter_ref(h264_dma_hal_context_t *hal);

/**
 * @brief  Configure un-encoder data DMA descriptor address
 *
 * @param  hal       Context of the HAL layer
 * @param  dsc_addr  Descriptor address
 */
void h264_dma_hal_cfg_yuv_dsc(h264_dma_hal_context_t *hal, uint32_t dsc_addr);

/**
 * @brief  Configure refrence frame DMA descriptor address
 *
 * @param  hal       Context of the HAL layer
 * @param  dsc_addr  Descriptor address
 * @param  buf_addr  Buffer address
 * @param  mb_width  The width of picture in macroblocks
 */
void h264_dma_hal_cfg_ref_dsc(h264_dma_hal_context_t *hal, uint32_t dsc_addr, uint32_t buf_addr, uint8_t mb_width);

/**
 * @brief  Configure the de-blocking filter temporary parameter DMA descriptor address
 *
 * @param  hal       Context of the HAL layer
 * @param  dsc_addr  Descriptor address
 */
void h264_dma_hal_cfg_dbtmp_dsc(h264_dma_hal_context_t *hal, uint32_t dsc_addr[2]);

/**
 * @brief  Configure the de-blocking filter lines DMA descriptor address
 *
 * @param  hal       Context of the HAL layer
 * @param  dsc_addr  Descriptor address
 */
void h264_dma_hal_cfg_db12_4_dsc(h264_dma_hal_context_t *hal, uint32_t dsc_addr[4]);

/**
 * @brief  Configure the encoder data DMA descriptor address
 *
 * @param  hal       Context of the HAL layer
 * @param  dsc_addr  Descriptor address
 */
void h264_dma_hal_cfg_bs_dsc(h264_dma_hal_context_t *hal, uint32_t dsc_addr);

/**
 * @brief  Configure the MVM DMA descriptor address
 *
 * @param  hal       Context of the HAL layer
 * @param  dsc_addr  Descriptor address
 */
void h264_dma_hal_cfg_mvm_dsc(h264_dma_hal_context_t *hal, uint32_t dsc_addr);

#ifdef __cplusplus
}
#endif
