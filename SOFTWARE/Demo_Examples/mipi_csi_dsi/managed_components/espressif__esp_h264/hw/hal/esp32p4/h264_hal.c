/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "h264_hal.h"

esp_h264_set_dev_t h264_hal_get_param_dev0(h264_hal_context_t *hal)
{
    return h264_ll_get_ctrl0(hal->dev);
}

esp_h264_set_dev_t h264_hal_get_param_dev1(h264_hal_context_t *hal)
{
    return h264_ll_get_ctrl1(hal->dev);
}

uint32_t h264_hal_get_intr_status(h264_hal_context_t *hal)
{
    return h264_ll_get_intr_st(hal->dev);
}

void h264_hal_ena_intr(h264_hal_context_t *hal, uint32_t intr)
{
    h264_ll_set_intr(hal->dev, intr);
}

void h264_hal_clear_intr_status(h264_hal_context_t *hal, uint32_t intr)
{
    h264_ll_clr_intr_st(hal->dev, intr);
}

void h264_hal_set_start(h264_hal_context_t *hal)
{
    h264_ll_set_frame_start(hal->dev);
}

void h264_hal_reset(h264_hal_context_t *hal)
{
    h264_ll_reset(hal->dev);
}

void h264_hal_set_rc_qp(esp_h264_set_dev_t device, bool ena, uint8_t qp_min, uint8_t qp_max)
{
    h264_ctrl_regs_t *ctrl = device;
    h264_ll_set_rc_en(ctrl, ena);
    h264_ll_set_rc_qp(ctrl, qp_min, qp_max);
}

void h264_hal_get_rc_qp(esp_h264_set_dev_t device, uint8_t *qp_min, uint8_t *qp_max)
{
    h264_ll_get_rc_qp(device, qp_min, qp_max);
}

void h264_hal_set_rc_rate_pred(esp_h264_set_dev_t device, uint32_t rate, uint32_t pred_mad)
{
    h264_ll_set_rc_rate_pred(device, rate, pred_mad);
}

void h264_hal_get_rc_bits_mad_qpsum(h264_hal_context_t *hal, uint32_t *enc_bits, uint32_t *mad, uint32_t *qp_sum)
{
    h264_ll_get_rc_param(hal->dev, enc_bits, mad, qp_sum);
}

void h264_hal_set_mv_mode(esp_h264_set_dev_t device, int8_t mv_mode, uint8_t mv_fmt)
{
    h264_dev_t *dev = H264_LL_GET_HW();
    bool ena = false;
    if (mv_mode >= 0) {
        ena = true;
        h264_ll_set_mvm(dev, (uint8_t)mv_mode, mv_fmt);
    }
    if ((uint32_t)device == (uint32_t)&dev->ctrl[0]) {
        h264_ll_set_mvm_en0(dev, ena);
    } else {
        h264_ll_set_mvm_en1(dev, ena);
    }
}

void h264_hal_get_mv_mode(esp_h264_set_dev_t device, int8_t *mv_mode, uint8_t *mv_fmt)
{
    h264_dev_t *dev = H264_LL_GET_HW();
    bool ena = true;
    if ((uint32_t)device == (uint32_t)&dev->ctrl[0]) {
        ena = h264_ll_get_mvm_en0(dev);
    } else {
        ena = h264_ll_get_mvm_en1(dev);
    }
    if (ena) {
        h264_ll_get_mvm(H264_LL_GET_HW(), (uint8_t *)mv_mode, mv_fmt);
        return;
    }
    *mv_mode = -1;
    *mv_fmt = 0;
}

uint32_t h264_hal_get_mvm_data_len(esp_h264_set_dev_t device)
{
    return h264_ll_get_mvm_data_len(H264_LL_GET_HW());
}

void h264_hal_set_gop(h264_hal_context_t *hal, uint8_t gop, bool gop_mode_en)
{
    h264_ll_set_gop(hal->dev, gop, gop_mode_en);
}

void h264_hal_dma_move_start(h264_hal_context_t *hal)
{
    h264_ll_dma_move_start(hal->dev);
}

uint32_t h264_hal_get_coded_len(h264_hal_context_t *hal)
{
    return h264_ll_get_coded_len(hal->dev);
}

void h264_hal_set_mbres(esp_h264_set_dev_t device, uint8_t mb_width, uint8_t mb_height)
{
    h264_ll_set_mb(device, mb_width, mb_height);
}

void h264_hal_get_mbres(esp_h264_set_dev_t device, uint8_t *mb_width, uint8_t *mb_height)
{
    h264_ll_get_mb(device, mb_width, mb_height);
}

void h264_hal_set_qp(esp_h264_set_dev_t device, uint8_t qp)
{
    h264_ll_set_qp(device, qp);
}

void h264_hal_get_qp(esp_h264_set_dev_t device, uint8_t *qp)
{
    *qp = h264_ll_get_qp(device);
}

void h264_hal_set_slice_header(h264_hal_context_t *hal, uint32_t header[3], uint32_t slice_bit_len)
{
    h264_ll_set_slice_header(hal->dev, header, slice_bit_len);
}

void h264_hal_set_roi_reg(esp_h264_set_dev_t device, bool ena, uint8_t x, uint8_t y, uint8_t len_x, uint8_t len_y, int8_t qp, uint8_t reg_idx)
{
    h264_ll_set_roi_reg(device, ena, x, y, len_x, len_y, qp, reg_idx);
}

void h264_hal_get_roi_reg(esp_h264_set_dev_t device, uint8_t *x, uint8_t *y, uint8_t *len_x, uint8_t *len_y, int8_t *qp, uint8_t reg_idx)
{
    if (h264_ll_get_roi_reg_en(device, reg_idx)) {
        h264_ll_get_roi_reg(device, x, y, len_x, len_y, qp, reg_idx);
        return;
    }
    *x = 0;
    *y = 0;
    *len_x = 0;
    *len_y = 0;
    *qp = 0;
}

void h264_hal_set_roi_mode(esp_h264_set_dev_t device, int8_t roi_mode, int8_t none_roi_delta_qp)
{
    if (roi_mode >= 0) {
        h264_ll_set_roi_cfg(device, roi_mode, none_roi_delta_qp);
        return;
    }
    h264_ll_disable_roi(device);
    for (uint8_t i = 0; i < 8; i++) {
        h264_ll_set_roi_reg(device, false, 0, 0, 0, 0, 0, 0);
    }
}

bool h264_hal_get_roi_mode(esp_h264_set_dev_t device, uint8_t *roi_mode, int8_t *none_roi_delta_qp)
{
    if (h264_ll_get_roi_en(device)) {
        h264_ll_get_roi_cfg(device, roi_mode, none_roi_delta_qp);
        return true;
    }
    return false;
}

bool h264_hal_get_bs_bit_overflow(h264_hal_context_t *hal)
{
    return h264_ll_get_bs_bit_overflow(hal->dev);
}

/*Init */
void h264_hal_init(h264_hal_context_t *hal, h264_hal_context_cfg_t *cfg)
{
    /*Enable H264 system clock*/
    h264_ll_reset_register(0);
    h264_ll_enable_bus_clock(0, true);
    h264_ll_enable_dma_clock(0, true);
    h264_ll_set_dma_clock_source(0, 0);

    /* Get hardware instance */
    hal->dev = H264_LL_GET_HW();

    /* Reset configure */
    h264_ll_set_sys(hal->dev);

    /* GOP configure */
    h264_ll_set_gop(hal->dev, cfg->gop, cfg->gop_mode_en);

    for (uint8_t i = 0; i < H264_SUP_MAX_CHANNEL; i++) {
        /*Decimate configure */
        h264_ll_set_decimate_score(&hal->dev->ctrl[i], cfg->cfg_ch[i].score_luma, cfg->cfg_ch[i].score_chroma);
        h264_ll_set_decimate_offset(&hal->dev->ctrl[i], cfg->cfg_ch[i].intra_luma_offset, cfg->cfg_ch[i].intra_chroma_offset, cfg->cfg_ch[i].inter_luma_offset, cfg->cfg_ch[i].inter_chroma_offset);
        /* IP cost configure */
        h264_ll_set_ip_cost_thres(&hal->dev->ctrl[i], cfg->cfg_ch[i].cost_thres);
        /* DQP configure */
        h264_ll_set_chroma_dqp(&hal->dev->ctrl[i], cfg->cfg_ch[i].chroma_delta_qp, cfg->cfg_ch[i].chroma_dc_delta_qp);
        /* Enable deblocking */
        h264_ll_set_db_bypass(&hal->dev->ctrl[i], cfg->cfg_ch[i].db_bypass);
        /* Disable ROI */
        h264_ll_set_roi_cfg(&hal->dev->ctrl[i], cfg->cfg_ch[i].roi_mode, cfg->cfg_ch[i].roi_none_roi_qp);
        /* Disable RC */
        h264_ll_set_rc_en(&hal->dev->ctrl[i], cfg->cfg_ch[i].rc_ena);
        /* Disable MVM */
        h264_ll_set_mvm_en0(hal->dev, cfg->cfg_ch[i].mvm_ena);
    }
    /* Dual stream configure */
    if (cfg->dual_stream_en) {
        h264_ll_set_dual_stream(hal->dev);
    }
}
