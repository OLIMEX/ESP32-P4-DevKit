/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "h264_dma_hal.h"
#include "esp_log.h"

void h264_dma_hal_init(h264_dma_hal_context_t *hal, h264_hal_dma_context_cfg_t *cfg)
{
    /* Get hardware instance */
    hal->dev = H264_DMA_LL_GET_HW();
    h264_dma_ll_set_exter_mem_addr(hal->dev, cfg->exter_addr_start[0], cfg->exter_addr_start[1], cfg->exter_addr_end[0], cfg->exter_addr_end[1]);
    h264_dma_ll_set_inter_mem_addr(hal->dev, cfg->inter_addr_start[0], cfg->inter_addr_start[1], cfg->inter_addr_end[0], cfg->inter_addr_end[1]);
    for (uint8_t i = 0; i < H264_DMA_IN_OUT_CH_NUM; i++) {
        h264_dma_ll_set_out_conf0(&hal->dev->dma_out_ch[i], cfg->out_ch_conf0[i]);
        h264_dma_ll_set_in_conf0(&hal->dev->dma_in_ch[i], cfg->in_ch_conf0[i]);
    }
    h264_dma_ll_set_in5_conf0(hal->dev, cfg->in_ch_conf0[5]);
    h264_dma_ll_set_all_burst_size(hal->dev, cfg->burst_size);
}

void h264_dma_hal_deinit(h264_dma_hal_context_t *hal)
{
    h264_dma_ll_set_all_link_stop(hal->dev);
}

uint32_t h264_dma_hal_get_bs_intr(h264_dma_hal_context_t *hal)
{
    return h264_dma_ll_get_in_intr(&hal->dev->dma_in_ch[4]);
}

void h264_dma_hal_clear_intr(h264_dma_hal_context_t *hal)
{
    h264_dma_ll_clear_all_intr(hal->dev);
}

void h264_dma_hal_start_yuv_dma(h264_dma_hal_context_t *hal)
{
    h264_dma_ll_set_out_link_stop(&hal->dev->dma_out_ch[0]);
    h264_dma_ll_reset_out(&hal->dev->dma_out_ch[0]);
    h264_dma_ll_set_out_link_start(&hal->dev->dma_out_ch[0]);
}

void h264_dma_hal_start_ref_dma(h264_dma_hal_context_t *hal)
{
    h264_dma_ll_set_out_link_stop(&hal->dev->dma_out_ch[1]);
    h264_dma_ll_reset_out(&hal->dev->dma_out_ch[1]);
    h264_dma_ll_set_out_link_start(&hal->dev->dma_out_ch[1]);
}

void h264_dma_hal_start_tx_dbtmp_dma(h264_dma_hal_context_t *hal)
{
    h264_dma_ll_set_out_link_stop(&hal->dev->dma_out_ch[2]);
    h264_dma_ll_reset_out(&hal->dev->dma_out_ch[2]);
    h264_dma_ll_set_out_link_start(&hal->dev->dma_out_ch[2]);
}

void h264_dma_hal_start_tx_db12_4_dma(h264_dma_hal_context_t *hal)
{
    h264_dma_ll_set_out_link_stop(&hal->dev->dma_out_ch[3]);
    h264_dma_ll_set_out_link_stop(&hal->dev->dma_out_ch[4]);
    h264_dma_ll_reset_in5(&hal->dev->dma_in_ch5);
    h264_dma_ll_set_out_link_start(&hal->dev->dma_out_ch[3]);
    h264_dma_ll_set_out_link_start(&hal->dev->dma_out_ch[4]);
}

void h264_dma_hal_start_rx_db12_4_dma(h264_dma_hal_context_t *hal)
{
    h264_dma_ll_set_in_link_stop(&hal->dev->dma_in_ch[0]);
    h264_dma_ll_set_in_link_stop(&hal->dev->dma_in_ch[1]);
    h264_dma_ll_reset_in(&hal->dev->dma_in_ch[0]);
    h264_dma_ll_reset_in(&hal->dev->dma_in_ch[1]);
    h264_dma_ll_set_in_link_start(&hal->dev->dma_in_ch[0]);
    h264_dma_ll_set_in_link_start(&hal->dev->dma_in_ch[1]);
}

void h264_dma_hal_start_rx_dbtmp_dma(h264_dma_hal_context_t *hal)
{
    h264_dma_ll_set_in_link_stop(&hal->dev->dma_in_ch[2]);
    h264_dma_ll_reset_in(&hal->dev->dma_in_ch[2]);
    h264_dma_ll_set_in_link_start(&hal->dev->dma_in_ch[2]);
}

void h264_dma_hal_start_rx_mvm_dma(h264_dma_hal_context_t *hal)
{
    h264_dma_ll_set_in_link_stop(&hal->dev->dma_in_ch[3]);
    h264_dma_ll_reset_in(&hal->dev->dma_in_ch[3]);
    h264_dma_ll_set_in_link_start(&hal->dev->dma_in_ch[3]);
}

void h264_dma_hal_start_rx_bs_dma(h264_dma_hal_context_t *hal)
{
    h264_dma_ll_set_in_link_stop(&hal->dev->dma_in_ch[4]);
    h264_dma_ll_reset_in(&hal->dev->dma_in_ch[4]);
    h264_dma_ll_set_in_link_start(&hal->dev->dma_in_ch[4]);
}

void h264_dma_hal_reset_counter_db(h264_dma_hal_context_t *hal)
{
    h264_dma_ll_reset_counter0(hal->dev);
    h264_dma_ll_reset_counter1(hal->dev);
}

void h264_dma_hal_reset_counter_dbtmp(h264_dma_hal_context_t *hal)
{
    h264_dma_ll_reset_counter2(hal->dev);
}

void h264_dma_hal_reset_counter_ref(h264_dma_hal_context_t *hal)
{
    h264_dma_ll_reset_counter5(hal->dev);
}

void h264_dma_hal_cfg_yuv_dsc(h264_dma_hal_context_t *hal, uint32_t dsc_addr)
{
    h264_dma_ll_set_out_link_addr(&hal->dev->dma_out_ch[0], dsc_addr);
}

void h264_dma_hal_cfg_ref_dsc(h264_dma_hal_context_t *hal, uint32_t dsc_addr, uint32_t buf_addr, uint8_t mb_width)
{
    h264_dma_ll_set_out_link_addr(&hal->dev->dma_out_ch[1], dsc_addr);
    h264_dma_ll_set_in5_block(hal->dev, buf_addr, mb_width);
}

void h264_dma_hal_cfg_dbtmp_dsc(h264_dma_hal_context_t *hal, uint32_t dsc_addr[2])
{
    h264_dma_ll_set_out_link_addr(&hal->dev->dma_out_ch[2], dsc_addr[0]);
    h264_dma_ll_set_in_link_addr(&hal->dev->dma_in_ch[2], dsc_addr[1]);
}

void h264_dma_hal_cfg_db12_4_dsc(h264_dma_hal_context_t *hal, uint32_t dsc_addr[4])
{
    h264_dma_ll_set_out_link_addr(&hal->dev->dma_out_ch[3], dsc_addr[0]);
    h264_dma_ll_set_in_link_addr(&hal->dev->dma_in_ch[0], dsc_addr[1]);
    h264_dma_ll_set_out_link_addr(&hal->dev->dma_out_ch[4], dsc_addr[2]);
    h264_dma_ll_set_in_link_addr(&hal->dev->dma_in_ch[1], dsc_addr[3]);
}

void h264_dma_hal_cfg_bs_dsc(h264_dma_hal_context_t *hal, uint32_t dsc_addr)
{
    h264_dma_ll_set_in_link_addr(&hal->dev->dma_in_ch[4], dsc_addr);
}

void h264_dma_hal_cfg_mvm_dsc(h264_dma_hal_context_t *hal, uint32_t dsc_addr)
{
    h264_dma_ll_set_in_link_addr(&hal->dev->dma_in_ch[3], dsc_addr);
}
