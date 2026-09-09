/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "h264_struct.h"
#include "soc/hp_sys_clkrst_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

#define H264_LL_GET_HW() ((h264_dev_t *)(0x50084000))
#define H264_INTR_MASK   (0xf)

/**
 * @brief  Reset the H264 module
 *
 * @param  group_id  Group ID
 */
static inline void h264_ll_reset_register(int group_id)
{
    (void)group_id;
    HP_SYS_CLKRST.hp_rst_en2.reg_rst_en_h264 = 1;
    HP_SYS_CLKRST.hp_rst_en2.reg_rst_en_h264 = 0;
}

/**
 * @brief  Enable the H264 clock for H264 encoder module
 *
 * @param  group_id  Group ID
 * @param  enable    true to enable, false to disable
 */
static inline void h264_ll_enable_bus_clock(int group_id, bool enable)
{
    (void)group_id;
    HP_SYS_CLKRST.soc_clk_ctrl1.reg_h264_sys_clk_en = enable;
}

/**
 * @brief  Enable h264 clock
 *
 * @param  group_id  Group ID
 * @param  enable    true to enable, false to disable
 */
static inline void h264_ll_enable_dma_clock(int group_id, bool enable)
{
    (void)group_id;
    HP_SYS_CLKRST.peri_clk_ctrl26.reg_h264_clk_en = enable;
}

/**
 * @brief  Set the clock source for the H264 interface
 *
 * @param  group_id  Group ID
 * @param  source    Clock source
 */
static inline void h264_ll_set_dma_clock_source(int group_id, int source)
{
    (void)group_id;
    (void)source;
    HP_SYS_CLKRST.peri_clk_ctrl26.reg_h264_clk_src_sel = 1;
}

/**
 * @brief  Get stream configure handle
 *
 * @param  h264  H.264 peripheral address
 *
 * @return
 *       - Stream 0 configure handle
 */
static inline h264_ctrl_regs_t *h264_ll_get_ctrl0(h264_dev_t *h264)
{
    return (h264_ctrl_regs_t *)&h264->ctrl[0];
}

/**
 * @brief  Get stream configure handle
 *
 * @param  h264  H.264 peripheral address
 *
 * @return
 *       - Stream 1 configure handle
 */
static inline h264_ctrl_regs_t *h264_ll_get_ctrl1(h264_dev_t *h264)
{
    return (h264_ctrl_regs_t *)&h264->ctrl[1];
}

/**
 * @brief  SYS configure
 *
 * @param  h264  H.264 peripheral address
 */
static inline void h264_ll_set_sys(h264_dev_t *h264)
{
    h264->conf.val = ~0;
    h264->sys_ctrl.frame_start = 0;
    h264->sys_ctrl.dma_move_start = 0;
    h264->sys_ctrl.sys_rst_pulse = 1;
    h264->int_ena.val = 0;
    h264->int_clr.val = ~0;
}

/**
 * @brief  Dual stream configure
 *
 * @param  h264  H.264 peripheral address
 */
static inline void h264_ll_set_dual_stream(h264_dev_t *h264)
{
    h264->gop_conf.dual_stream_mode = 1;
    h264->sys_ctrl.frame_mode = 1;
}

/**
 * @brief  Group of picture(GOP) configure
 *
 * @param  h264         H.264 peripheral address
 * @param  gop          Group of picture(GOP)
 * @param  gop_mode_en  GOP mode enable
 */
static inline void h264_ll_set_gop(h264_dev_t *h264, uint8_t gop, bool gop_mode_en)
{
    h264->gop_conf.gop_num = gop;
    h264->sys_ctrl.frame_mode = !gop_mode_en;
    h264->gop_conf.dual_stream_mode = 0;
}

/**
 * @brief  Get group of picture(GOP)
 *
 * @param  h264  H.264 peripheral address
 * @return
 */
static inline uint8_t h264_ll_get_gop(h264_dev_t *h264)
{
    return h264->gop_conf.gop_num;
}

/**
 * @brief  Reset H.264 system
 *
 * @param  h264  H.264 peripheral address
 */
static inline void h264_ll_reset(h264_dev_t *h264)
{
    h264->sys_ctrl.sys_rst_pulse = 1;
}

/**
 * @brief  Start H.264 system
 *
 * @param  h264  H.264 peripheral address
 */
static inline void h264_ll_set_frame_start(h264_dev_t *h264)
{
    h264->sys_ctrl.frame_start = 1;
}

/**
 * @brief  Start move data to memory
 *
 * @param  h264  H.264 peripheral address
 */
static inline void h264_ll_dma_move_start(h264_dev_t *h264)
{
    h264->sys_ctrl.dma_move_start = 1;
}

/**
 * @brief  Set width and height of picture in macroblocks
 *
 * @param  ctrl       Stream configure handle
 * @param  mb_width   The width of picture in macroblocks
 * @param  mb_height  The height of picture in macroblock
 */
static inline void h264_ll_set_mb(volatile h264_ctrl_regs_t *ctrl, uint8_t mb_width, uint8_t mb_height)
{
    ctrl->sys_mb_res.sys_total_mb_y = mb_height;
    ctrl->sys_mb_res.sys_total_mb_x = mb_width;
    ctrl->sys_conf.db_tmp_ready_trigger_mb_num = mb_width >> 1;
    ctrl->sys_conf.rec_ready_trigger_mb_lines = 4; // it is less than mb_height
}

/**
 * @brief  Get width and height of picture in macroblocks
 *
 * @param  ctrl       Stream configure handle
 * @param  mb_width   The width of picture in macroblocks
 * @param  mb_height  The height of picture in macroblock
 */
static inline void h264_ll_get_mb(volatile h264_ctrl_regs_t *ctrl, uint8_t *mb_width, uint8_t *mb_height)
{
    *mb_height = ctrl->sys_mb_res.sys_total_mb_y;
    *mb_width = ctrl->sys_mb_res.sys_total_mb_x;
}

/**
 * @brief  Decimate score configure
 *
 * @param  ctrl          Stream configure handle
 * @param  score_luma    Luma MB decimate score
 * @param  score_chroma  Chroma MB decimate score
 */
static inline void h264_ll_set_decimate_score(volatile h264_ctrl_regs_t *ctrl, uint8_t score_luma, uint8_t score_chroma)
{
    ctrl->deci_score.c_deci_score = score_chroma;
    ctrl->deci_score.l_deci_score = score_luma;
}

/**
 * @brief  Decimate offset configure
 *
 * @param  ctrl                          Stream configure handle
 * @param  intra_luma_offset             Intra luma MB decimate score offset
 * @param  intra_chroma_offsetIntra      Chroma MB decimate score offset
 * @param  inter_luma_offset             Predicted luma MB decimate score offset
 * @param  inter_chroma_offsetPredicted  Chroma MB decimate score offset
 */
static inline void h264_ll_set_decimate_offset(volatile h264_ctrl_regs_t *ctrl, uint8_t intra_luma_offset, uint8_t intra_chroma_offset, uint8_t inter_luma_offset, uint8_t inter_chroma_offset)
{
    ctrl->deci_score_offset.i16x16_deci_score_offset = intra_luma_offset;
    ctrl->deci_score_offset.i_chroma_deci_score_offset = intra_chroma_offset;
    ctrl->deci_score_offset.p16x16_deci_score_offset = inter_luma_offset;
    ctrl->deci_score_offset.p_chroma_deci_score_offset = inter_chroma_offset;
}

/**
 * @brief  Rate control enable
 *
 * @param  ctrl  Stream configure handle
 * @param  ena   True: enable false: disable
 */
static inline void h264_ll_set_rc_en(volatile h264_ctrl_regs_t *ctrl, bool ena)
{
    ctrl->rc_conf0.mb_rate_ctrl_en = ena;
}

/**
 * @brief  Set rate control quantization parameter (QP)
 *
 * @param  ctrl    Stream configure handle
 * @param  qp_min  Mininum of quantization parameter(QP). range in [0, 51]
 * @param  qp_max  Maxinum of quantization parameter(QP). range in [0, 51]
 */
static inline void h264_ll_set_rc_qp(volatile h264_ctrl_regs_t *ctrl, uint8_t qp_min, uint8_t qp_max)
{
    ctrl->rc_conf1.qp_min = qp_min;
    ctrl->rc_conf1.qp_max = qp_max;
}

/**
 * @brief  Get rate control quantization parameter (QP)
 *
 * @param  ctrl    Stream configure handle
 * @param  qp_min  Mininum of quantization parameter(QP). range in [0, 51]
 * @param  qp_max  Maxinum of quantization parameter(QP). range in [0, 51]
 */
static inline void h264_ll_get_rc_qp(volatile h264_ctrl_regs_t *ctrl, uint8_t *qp_min, uint8_t *qp_max)
{
    *qp_min = ctrl->rc_conf1.qp_min;
    *qp_max = ctrl->rc_conf1.qp_max;
}

/**
 * @brief  Set rate control parameter
 *
 * @param  ctrl      Stream configure handle
 * @param  rate      The rate is between target bit and predicted bit.
 * @param  pred_mad  Predicted mean absolute difference (MAD)
 *
 */
static inline void h264_ll_set_rc_rate_pred(volatile h264_ctrl_regs_t *ctrl, uint32_t rate, uint32_t pred_mad)
{
    ctrl->rc_conf0.rate_ctrl_u = rate;
    ctrl->rc_conf1.mad_frame_pred = pred_mad;
}

/**
 * @brief  Chroma QP configure
 *
 * @param  ctrl                Stream configure handle
 * @param  chroma_delta_qp     Chroma QP offset based on luma QP
 * @param  chroma_dc_delta_qp  Chroma DC QP offset based on Chroma QP
 */
static inline void h264_ll_set_chroma_dqp(volatile h264_ctrl_regs_t *ctrl, uint8_t chroma_delta_qp, uint8_t chroma_dc_delta_qp)
{
    ctrl->rc_conf1.chroma_qp_delta = chroma_delta_qp;
    ctrl->rc_conf1.chroma_dc_qp_delta = chroma_dc_delta_qp;
}

/**
 * @brief  Intra frame cost threshold configure
 *
 * @param  ctrl   Stream configure handle
 * @param  thres  Intra frame cost threshold
 */
static inline void h264_ll_set_ip_cost_thres(volatile h264_ctrl_regs_t *ctrl, uint8_t thres)
{
    ctrl->sys_conf.intra_cost_cmp_offset = thres;
}

/**
 * @brief  Set quantizer parameter(QP)
 *
 * @param  ctrl  Stream configure handle
 * @param  qp    Quantizer parameter(QP)
 */
static inline void h264_ll_set_qp(volatile h264_ctrl_regs_t *ctrl, uint8_t qp)
{
    ctrl->rc_conf0.qp = qp;
}

/**
 * @brief  Get quantizer parameter(QP)
 *
 * @param  ctrl  Stream configure handle
 * @return
 *       - Quantizer parameter(QP)
 */
static inline uint8_t h264_ll_get_qp(volatile h264_ctrl_regs_t *ctrl)
{
    return ctrl->rc_conf0.qp;
}

/**
 * @brief  Enable de-blocking
 *
 * @param  ctrl  Stream configure handle
 * @param  ena   True: enable the region. false: disable
 */
static inline void h264_ll_set_db_bypass(volatile h264_ctrl_regs_t *ctrl, bool ena)
{
    ctrl->db_bypass.bypass_db_filter = ena;
}

/**
 * @brief  Set range of interesting (ROI) region
 *
 * @param  ctrl     Stream configure handle
 * @param  ena      True: enable the region. false: disable the region
 * @param  x        Start position in horizontal direction. units macroblock size
 * @param  y        Start position in vertical direction. units macroblock size
 * @param  len_x    ROI region length in horizontal direction. units macroblock size
 * @param  len_y    ROI region length in vertical direction. units macroblock size
 * @param  qp       Quantization parameter(QP).
 * @param  reg_idx  The index of ROI region. It's must less than 8.
 */
static inline void h264_ll_set_roi_reg(volatile h264_ctrl_regs_t *ctrl, bool ena, uint8_t x, uint8_t y, uint8_t xlen, uint8_t ylen, int8_t qp, uint8_t reg_idx)
{
    ctrl->roi_region[reg_idx].roi_region_en = ena;
    ctrl->roi_region[reg_idx].roi_region_x = x;
    ctrl->roi_region[reg_idx].roi_region_y = y;
    ctrl->roi_region[reg_idx].roi_region_x_len = xlen;
    ctrl->roi_region[reg_idx].roi_region_y_len = ylen;
    switch (reg_idx) {
    case 0:
        ctrl->roi_region0_3_qp.roi_region0_qp = qp;
        break;
    case 1:
        ctrl->roi_region0_3_qp.roi_region1_qp = qp;
        break;
    case 2:
        ctrl->roi_region0_3_qp.roi_region2_qp = qp;
        break;
    case 3:
        ctrl->roi_region0_3_qp.roi_region3_qp = qp;
        break;
    case 4:
        ctrl->roi_region4_7_qp.roi_region4_qp = qp;
        break;
    case 5:
        ctrl->roi_region4_7_qp.roi_region5_qp = qp;
        break;
    case 6:
        ctrl->roi_region4_7_qp.roi_region6_qp = qp;
        break;
    case 7:
        ctrl->roi_region4_7_qp.roi_region7_qp = qp;
        break;
    }
}

/**
 * @brief  Get range of interesting (ROI) region enable register
 *
 * @param  ctrl     Stream configure handle
 * @param  reg_idx  The index of ROI region. It's must less than 8.
 *
 * @return
 *       - True   Enable
 *       - False  Disable
 */
static inline bool h264_ll_get_roi_reg_en(volatile h264_ctrl_regs_t *ctrl, uint8_t reg_idx)
{
    return ctrl->roi_region[reg_idx].roi_region_en;
}

/**
 * @brief  Get range of interesting (ROI) region
 *
 * @param  ctrl     Stream configure handle
 * @param  ena      True: enable the region. false: disable the region
 * @param  x        Start position in horizontal direction. units macroblock size
 * @param  y        Start position in vertical direction. units macroblock size
 * @param  len_x    ROI region length in horizontal direction. units macroblock size
 * @param  len_y    ROI region length in vertical direction. units macroblock size
 * @param  qp       Quantization parameter(QP)
 * @param  reg_idx  The index of ROI region. It's must less than 8.
 */
static inline void h264_ll_get_roi_reg(volatile h264_ctrl_regs_t *ctrl, uint8_t *x, uint8_t *y, uint8_t *xlen, uint8_t *ylen, int8_t *qp, uint8_t reg_idx)
{
    *x = ctrl->roi_region[reg_idx].roi_region_x;
    *y = ctrl->roi_region[reg_idx].roi_region_y;
    *xlen = ctrl->roi_region[reg_idx].roi_region_x_len;
    *ylen = ctrl->roi_region[reg_idx].roi_region_y_len;
    switch (reg_idx) {
    case 0:
        *qp = ctrl->roi_region0_3_qp.roi_region0_qp;
        break;
    case 1:
        *qp = ctrl->roi_region0_3_qp.roi_region1_qp;
        break;
    case 2:
        *qp = ctrl->roi_region0_3_qp.roi_region2_qp;
        break;
    case 3:
        *qp = ctrl->roi_region0_3_qp.roi_region3_qp;
        break;
    case 4:
        *qp = ctrl->roi_region4_7_qp.roi_region4_qp;
        break;
    case 5:
        *qp = ctrl->roi_region4_7_qp.roi_region5_qp;
        break;
    case 6:
        *qp = ctrl->roi_region4_7_qp.roi_region6_qp;
        break;
    case 7:
        *qp = ctrl->roi_region4_7_qp.roi_region7_qp;
        break;
    }
}

/**
 * @brief  Disable range of interesting (ROI)
 *
 * @param  ctrl  Stream configure handle
 *
 */
static inline void h264_ll_disable_roi(volatile h264_ctrl_regs_t *ctrl)
{
    ctrl->roi_config.roi_en = 0;
}

/**
 * @brief  Get range of interesting (ROI) enable register
 *
 * @param  ctrl  Stream configure handle
 *
 * @return
 *       - True   ROI enable
 *       - False  Disable
 *
 */
static inline bool h264_ll_get_roi_en(volatile h264_ctrl_regs_t *ctrl)
{
    return ctrl->roi_config.roi_en;
}

/**
 * @brief  Set range of interesting (ROI) mode
 *
 * @param  ctrl         Stream configure handle
 * @param  roi_mode     Range of interesting mode
 *                      0: The QP of ROI is fixed.
 *                      1: The quantization parameter(QP) of ROI is summation of delta QP and original QP.
 * @param  none_roi_qp  Delta quantization parameter(QP) of none ROI region. range in [-51, 51]
 *
 */
static inline void h264_ll_set_roi_cfg(volatile h264_ctrl_regs_t *ctrl, uint8_t mode, int8_t none_roi_qp)
{
    ctrl->roi_config.roi_en = 1;
    ctrl->roi_config.roi_mode = mode;
    ctrl->no_roi_region_qp_offset.no_roi_region_qp = none_roi_qp;
}

/**
 * @brief  Get range of interesting (ROI) mode
 *
 * @param  ctrl         Stream configure handle
 * @param  roi_mode     Range of interesting mode
 *                      0: The QP of ROI is fixed.
 *                      1: The quantization parameter(QP) of ROI is summation of delta QP and original QP.
 * @param  none_roi_qp  Delta quantization parameter(QP) of none ROI region
 *
 */
static inline void h264_ll_get_roi_cfg(volatile h264_ctrl_regs_t *ctrl, uint8_t *mode, int8_t *none_roi_qp)
{
    *mode = ctrl->roi_config.roi_mode;
    *none_roi_qp = ctrl->no_roi_region_qp_offset.no_roi_region_qp;
}

/**
 * @brief  Get rate control parameter
 *
 * @param  h264      H.264 peripheral address
 * @param  enc_bits  Encoder data bit length
 * @param  mad       Mean absolute difference (MAD) for the frame
 * @param  qp_sum    The sum of QP
 */
static inline void h264_ll_get_rc_param(h264_dev_t *h264, uint32_t *encbits, uint32_t *mad, uint32_t *qp_sum)
{
    *mad = h264->rc_status0.frame_mad_sum;
    *encbits = h264->rc_status1.frame_enc_bits;
    *qp_sum = h264->rc_status2.frame_qp_sum;
}

/**
 * @brief  Set slice header
 *         If the header last byte is less than 8 bits,round it up to one byte.
 *         If the header is less than 3 bytes, write zero for the rest bits.
 *
 * @param  h264           H.264 peripheral address
 * @param  header         The last three bytes of slice header
 * @param  slice_bit_len  `header` actual bit length
 */
static inline void h264_ll_set_slice_header(h264_dev_t *h264, uint32_t header[3], uint16_t slice_bit_len)
{
    h264->slice_header[0].val = header[0];
    h264->slice_header[1].val = header[1];
    h264->slice_header_remain.slice_remain_bit = (header[2] & 0xff);
    h264->slice_header_remain.slice_remain_bitlength = slice_bit_len & 0x7;
    h264->slice_header_byte_length.slice_byte_length = slice_bit_len >> 3;
}

/**
 * @brief  Get H.264 interrput status
 *
 * @param  h264  H.264 peripheral address
 * @return
 */
static inline uint32_t h264_ll_get_intr_st(h264_dev_t *h264)
{
    return (h264->int_st.val & H264_INTR_MASK);
}
/**
 * @brief  Set H.264 interrput status
 *
 * @param  h264  H.264 peripheral address
 * @param  intr  H.264 interrput status
 */
static inline void h264_ll_set_intr(h264_dev_t *h264, uint32_t intr)
{
    h264->int_ena.val = intr;
}
/**
 * @brief  Clear H.264 interrput status
 *
 * @param  h264  H.264 peripheral address
 * @param  intr  H.264 interrput status
 */
static inline void h264_ll_clr_intr_st(h264_dev_t *h264, uint32_t intr)
{
    h264->int_clr.val |= intr;
}

/**
 * @brief  Enable the motion vector (MV) output
 *
 * @param  h264     H.264 peripheral address
 * @param  mv_ena0  Ture: enable false: disable
 */
static inline void h264_ll_set_mvm_en0(h264_dev_t *h264, bool mv_ena0)
{
    h264->mv_merge_config.a_mv_merge_en = mv_ena0;
}

/**
 * @brief  Enable the motion vector (MV) output
 *
 * @param  h264     H.264 peripheral address
 * @param  mv_ena1  Ture: enable false: disable
 */
static inline void h264_ll_set_mvm_en1(h264_dev_t *h264, bool mv_ena1)
{
    h264->mv_merge_config.b_mv_merge_en = mv_ena1;
}

/**
 * @brief  Get the motion vector (MV) enable register
 *
 * @param  h264  H.264 peripheral address
 *
 * @return
 *       - Ture   Enable
 *       - False  Disable
 */
static inline bool h264_ll_get_mvm_en0(h264_dev_t *h264)
{
    return h264->mv_merge_config.a_mv_merge_en;
}

/**
 * @brief  Get the motion vector (MV) enable register
 *
 * @param  h264  H.264 peripheral address
 *
 * @return
 *       - Ture   Enable
 *       - False  Disable
 */
static inline bool h264_ll_get_mvm_en1(h264_dev_t *h264)
{
    return h264->mv_merge_config.b_mv_merge_en;
}

/**
 * @brief  Set motion vector(MV) mode
 *
 * @param  h264     H.264 peripheral address
 * @param  mv_mode  Motion vector(MV) mode
 *                  0: The 16 * 16 macro block MV data collection
 *                  1: If sub-macro-block is exist, the MV data is minimum of sub-macro-block MV data.
 *                     Otherwise it's result of 16 * 16 macro block MV data.
 *                  2: If sub-macro-block is exist, the MV data is maximum of sub-macro-block MV data.
 *                     Otherwise it's result of 16 * 16 macro block MV data.
 * @param  mv_fmt   Motion vector(MV) format
 *                  0: Output all MV data expect zero
 *                  1: Output horizontal or vertical direction absolute of MV data gather than or equal 4.
 */
static inline void h264_ll_set_mvm(h264_dev_t *h264, uint8_t mv_mode, uint8_t mv_fmt)
{
    h264->mv_merge_config.mv_merge_type = mv_mode;
    h264->mv_merge_config.int_mv_out_en = mv_fmt;
}

/**
 * @brief  Get motion vector(MV) mode
 *
 * @param  h264     H.264 peripheral address
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
static inline void h264_ll_get_mvm(h264_dev_t *h264, uint8_t *mv_mode, uint8_t *mv_fmt)
{
    *mv_mode = h264->mv_merge_config.mv_merge_type;
    *mv_fmt = h264->mv_merge_config.int_mv_out_en;
}

/**
 * @brief  Get motion vector(MV) buffer actual length
 *
 * @param  h264  H.264 peripheral address
 * @return
 */
static inline uint32_t h264_ll_get_mvm_data_len(h264_dev_t *h264)
{
    return h264->mv_merge_config.mb_valid_num;
}

/**
 * @brief  Get encoder data length
 *
 * @param  h264  H.264 peripheral address
 * @return
 */
static inline uint32_t h264_ll_get_coded_len(h264_dev_t *h264)
{
    return h264->frame_code_length.frame_code_length;
}

/**
 * @brief  Get bs error
 *
 * @param  h264  H.264 peripheral address
 * @return
 */
static inline bool h264_ll_get_bs_bit_overflow(h264_dev_t *h264)
{
    return h264->debug_info1.bs_buffer_debug_state;
}

#ifdef __cplusplus
}
#endif
