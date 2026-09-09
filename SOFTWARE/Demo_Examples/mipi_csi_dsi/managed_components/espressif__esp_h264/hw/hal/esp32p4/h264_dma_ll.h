/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "h264_dma_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

#define H264_DMA_LL_GET_HW()            ((h264_dma_dev_t *)(0x500A7000))
#define H264_DMA_MACRO_SIZE             (16)
#define H264_DMA_HALF_MACRO_SIZE        (8)
#define H264_DMA_12_LINES               (12)
#define H264_DMA_4_LINES                (4)
#define H264_DMA_DB_12_LINES_Y          (H264_DMA_MACRO_SIZE * H264_DMA_12_LINES)
#define H264_DMA_DB_4_LINES_Y           (H264_DMA_MACRO_SIZE * H264_DMA_4_LINES)
#define H264_DMA_DB_ROW_LENGTH_UV       (H264_DMA_HALF_MACRO_SIZE * H264_DMA_4_LINES * 2)
#define H264_DMA_DB_12_LINES_ROW_LENGTH (H264_DMA_DB_12_LINES_Y + H264_DMA_DB_ROW_LENGTH_UV)
#define H264_DMA_DB_4_LINES_ROW_LENGTH  (H264_DMA_DB_4_LINES_Y + H264_DMA_DB_ROW_LENGTH_UV)
#define H264_DMA_3_LINES                (3)
#define H264_DMA_2D_ENABLE              (1)
#define H264_DMA_2D_DISABLE             (0)
#define H264_DMA_MODE0                  (0)
#define H264_DMA_MODE1                  (1)
#define H264_DMA_OWNER_CPU              (0)
#define H264_DMA_OWNER_H264             (1)
#define H264_DMA_EOF_CONTINUE           (0)
#define H264_DMA_EOF_END                (1)
#define H264_DMA_SIZE_BIT               (14)
#define H264_DMA_MAX_SIZE               (0x3fff)
#define H264_DMA_IN_OUT_CH_NUM          (5)

/**
 * @brief  Type of DMA descriptor
 */
typedef struct h264_dma_desc h264_dma_desc_t;
struct h264_dma_desc {
    volatile uint32_t vb : 14,  /*!< Number of vertical pixels of the block */
             hb : 14,  /*!< Number of horizontal pixels of the block */
             err_eof : 1,   /*!< Whether the received buffer contains error */
             dma_2d_en : 1,   /*!< Whether to enable 2D functionality */
             eof : 1,   /*!< Whether the descriptor is the last one in the link */
             owner : 1;   /*!< Who is allowed to access the buffer that this descriptor points to. 0: CPU 1: H.264 */
    volatile uint32_t va : 14,  /*!< Number of vertical pixels of the picture */
             ha : 15,  /*!< Number of horizontal pixels of the picture */
             mode : 1,   /*!< Data block read/write mode */
             rscv30 : 2;   /*!< Reserved */
    volatile uint8_t *buf;      /*!< If dma_2d_en = 1, pointer to the buffer of HA * VA picture
                                     If dma_2d_en = 0, pointer to the buffer*/
    h264_dma_desc_t *next_desc; /*!< Pointer to the next descriptor. Set to NULL if the descriptor is the last one, e.g. eof=1 */
};

/**
 * @brief  Set extern memory start and end address
 *
 * @param  dma  DMA peripheral address
 * @param  as0  The start address of first block extern memory
 * @param  as1  The start address of second block  extern memory
 * @param  ae0  The end address of first block extern memory
 * @param  ae1  The end address of second block extern memory
 */
static inline void h264_dma_ll_set_exter_mem_addr(h264_dma_dev_t *dma, uint32_t as0, uint32_t as1, uint32_t ae0, uint32_t ae1)
{
    dma->exter_mem_addr[0].start.val = as0;
    dma->exter_mem_addr[1].start.val = as1;
    dma->exter_mem_addr[0].end.val = ae0;
    dma->exter_mem_addr[1].end.val = ae1;
}

/**
 * @brief  Set internal memory start and end address
 *
 * @param  dma  DMA peripheral address
 * @param  as0  The start address of first block internal memory
 * @param  as1  The start address of second block internal memory
 * @param  ae0  The end address of first block internal memory
 * @param  ae1  The end address of second block internal memory
 */
static inline void h264_dma_ll_set_inter_mem_addr(h264_dma_dev_t *dma, uint32_t as0, uint32_t as1, uint32_t ae0, uint32_t ae1)
{
    dma->inter_mem_addr[0].start.val = as0;
    dma->inter_mem_addr[1].start.val = as1;
    dma->inter_mem_addr[0].end.val = ae0;
    dma->inter_mem_addr[1].end.val = ae1;
}

/**
 * @brief  Configure TX channel
 *
 * @param  out   Tx channel address
 * @param  cfg0  Configure word
 */
static inline void h264_dma_ll_set_out_conf0(volatile h264_dma_out_chn_regs_t *out, uint32_t cfg0)
{
    out->conf0.val = cfg0;
}

/**
 * @brief  Configure RX channel
 *
 * @param  in    Rx channel address
 * @param  cfg0  Configure word
 */
static inline void h264_dma_ll_set_in_conf0(volatile h264_dma_in_chn_regs_t *in, uint32_t cfg0)
{
    in->conf0.val = cfg0;
}

/**
 * @brief  Configure RX channel 5
 *
 * @param  dma   DMA peripheral address
 * @param  cfg0  Configure word
 */
static inline void h264_dma_ll_set_in5_conf0(h264_dma_dev_t *dma, uint32_t cfg0)
{
    dma->dma_in_ch5.conf0.val = cfg0;
}

/**
 * @brief  Set DMA RX channel burst reading data length in bytes
 *
 * @param  dma         DMA peripheral address
 * @param  burst_size  Burst size
 */
static inline void h264_dma_ll_set_all_burst_size(h264_dma_dev_t *dma, uint8_t burst_size)
{
    for (uint8_t i = 0; i < H264_DMA_IN_OUT_CH_NUM; i++) {
        dma->dma_in_ch[i].conf0.in_mem_burst_length = burst_size;
        dma->dma_out_ch[i].conf0.out_mem_burst_length = burst_size;
    }
    dma->dma_in_ch5.conf0.in_mem_burst_length = burst_size;
}

/**
 * @brief  Stop DMA link
 *
 * @param  dma  DMA peripheral address
 */
static inline void h264_dma_ll_set_all_link_stop(h264_dma_dev_t *dma)
{
    for (uint8_t i = 0; i < 3; i++) {
        dma->dma_in_ch[i].link_conf.inlink_stop = 1;
        dma->dma_in_ch[i].conf0.in_rst = 1;
        dma->dma_in_ch[i].conf0.in_rst = 0;
    }
    dma->dma_in_ch5.conf0.in_rst = 1;
    dma->dma_in_ch5.conf0.in_rst = 0;
    for (uint8_t i = 0; i < H264_DMA_IN_OUT_CH_NUM; i++) {
        dma->dma_out_ch[i].link_conf.outlink_stop = 1;
        dma->dma_out_ch[i].conf0.out_rst = 1;
        dma->dma_out_ch[i].conf0.out_rst = 0;
    }
}

/**
 * @brief  Clear the DMA interrput
 *
 * @param  dma  DMA peripheral address
 */
static inline void h264_dma_ll_clear_all_intr(h264_dma_dev_t *dma)
{
    for (uint8_t i = 0; i < H264_DMA_IN_OUT_CH_NUM; i++) {
        dma->dma_in_ch[i].int_clr.val = ~0;
        dma->dma_out_ch[i].int_clr.val = ~0;
    }
    dma->dma_in_ch5.int_clr.val = ~0;
}

/**
 * @brief  Get the DMA RX interrput
 *
 * @param  in  Rx channel address
 *
 * @return
 *       - The RX interrupt
 */
static inline uint32_t h264_dma_ll_get_in_intr(volatile h264_dma_in_chn_regs_t *in)
{
    return in->int_raw.val;
}

/**
 * @brief  Reset TX channel
 *
 * @param  out  Tx channel address
 */
static inline void h264_dma_ll_reset_out(volatile h264_dma_out_chn_regs_t *out)
{
    out->conf0.out_cmd_disable = 1;
    while (!out->state.out_reset_avail);
    out->conf0.out_rst = 1;
    out->conf0.out_rst = 0;
    out->conf0.out_cmd_disable = 0;
}

/**
 * @brief  Reset RX channel
 *
 * @param  in  Rx channel address
 */
static inline void h264_dma_ll_reset_in(volatile h264_dma_in_chn_regs_t *in)
{
    in->conf0.in_cmd_disable = 1;
    while (!in->state.in_reset_avail);
    in->conf0.in_rst = 1;
    in->conf0.in_rst = 0;
    in->conf0.in_cmd_disable = 0;
}

/**
 * @brief  Reset RX channel 5
 *
 * @param  in  Rx channel address
 */
static inline void h264_dma_ll_reset_in5(volatile h264_dma_in_ch5_regs_t *in)
{
    in->conf0.in_cmd_disable = 1;
    while (!in->state.in_reset_avail);
    in->conf0.in_rst = 1;
    in->conf0.in_rst = 0;
    in->conf0.in_cmd_disable = 0;
}

/**
 * @brief  Stop the RX link
 *
 * @param  in  Rx channel address
 */
static inline void h264_dma_ll_set_in_link_stop(volatile h264_dma_in_chn_regs_t *in)
{
    in->link_conf.inlink_stop = 1;
}

/**
 * @brief  Restart a new link right after the last descriptor
 *
 * @param  in  Rx channel address
 */
static inline void h264_dma_ll_set_in_link_start(volatile h264_dma_in_chn_regs_t *in)
{
    in->link_conf.inlink_start = 1;
}

/**
 * @brief  Restart a new link right after the last descriptor
 *
 * @param  out  Tx channel address
 */
static inline void h264_dma_ll_set_out_link_start(volatile h264_dma_out_chn_regs_t *out)
{
    out->link_conf.outlink_start = 1;
}

/**
 * @brief  Stop the TX link
 *
 * @param  out  Tx channel address
 */
static inline void h264_dma_ll_set_out_link_stop(volatile h264_dma_out_chn_regs_t *out)
{
    out->link_conf.outlink_stop = 1;
}

/**
 * @brief  Reset counter 0
 *
 * @param  dma  DMA peripheral address
 */
static inline void h264_dma_ll_reset_counter0(h264_dma_dev_t *dma)
{
    dma->counter_rst.rx_ch0_exter_counter_rst = 1;
    dma->counter_rst.rx_ch0_exter_counter_rst = 0;
}

/**
 * @brief  Reset counter 1
 *
 * @param  dma  DMA peripheral address
 */
static inline void h264_dma_ll_reset_counter1(h264_dma_dev_t *dma)
{
    dma->counter_rst.rx_ch1_exter_counter_rst = 1;
    dma->counter_rst.rx_ch1_exter_counter_rst = 0;
}

/**
 * @brief  Reset counter 2
 *
 * @param  dma  DMA peripheral address
 */
static inline void h264_dma_ll_reset_counter2(h264_dma_dev_t *dma)
{
    dma->counter_rst.rx_ch2_inter_counter_rst = 1;
    dma->counter_rst.rx_ch2_inter_counter_rst = 0;
}

/**
 * @brief  Reset counter 5
 *
 * @param  dma  DMA peripheral address
 */
static inline void h264_dma_ll_reset_counter5(h264_dma_dev_t *dma)
{
    dma->counter_rst.rx_ch5_inter_counter_rst = 1;
    dma->counter_rst.rx_ch5_inter_counter_rst = 0;
}

/**
 * @brief  Set the descriptor link base address for TX channel
 *
 * @param  out   Tx channel address
 * @param  addr  Address word
 */
static inline void h264_dma_ll_set_out_link_addr(volatile h264_dma_out_chn_regs_t *out, uint32_t addr)
{
    out->link_addr.val = addr;
}

/**
 * @brief  Set the descriptor link base address for RX channel
 *
 * @param  in    Rx channel address
 * @param  addr  Address word
 */
static inline void h264_dma_ll_set_in_link_addr(volatile h264_dma_in_chn_regs_t *in, uint32_t addr)
{
    in->link_addr.val = addr;
}

/**
 * @brief  Set the descriptor link base address for RX channel
 *
 * @param  dma       DMA peripheral address
 * @param  buf       Start address
 * @param  mb_width  The width of picture in macroblocks.
 */
static inline void h264_dma_ll_set_in5_block(h264_dma_dev_t *dma, uint32_t buf, int mb_width)
{
    dma->dma_in_ch5.conf1.block_start_addr = buf;
    dma->dma_in_ch5.conf2.block_row_length_12line = H264_DMA_DB_12_LINES_ROW_LENGTH * mb_width;
    dma->dma_in_ch5.conf2.block_row_length_4line = H264_DMA_DB_4_LINES_ROW_LENGTH * mb_width;
    dma->dma_in_ch5.conf3.block_length_12line = H264_DMA_DB_12_LINES_ROW_LENGTH;
    dma->dma_in_ch5.conf3.block_length_4line = H264_DMA_DB_4_LINES_ROW_LENGTH;
}

#ifdef __cplusplus
}
#endif
