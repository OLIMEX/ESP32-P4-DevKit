/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_h264_enc_param_hw.h"
#include "esp_h264_check.h"

static const char *TAG = "H264_ENC.IF.HW.PARAM";

esp_h264_err_t esp_h264_enc_hw_cfg_roi(esp_h264_enc_param_hw_handle_t handle, esp_h264_enc_roi_cfg_t cfg)
{
    ESP_H264_RET_ON_FALSE(handle, ESP_H264_ERR_ARG, TAG, "Invalid h264 parameter");
    ESP_H264_RET_ON_FALSE((cfg.roi_mode < ESP_H264_ROI_MODE_INVALID) && (cfg.roi_mode >= ESP_H264_ROI_MODE_DISABLE), ESP_H264_ERR_ARG, TAG, "ROI mode error ");
    ESP_H264_RET_ON_FALSE((cfg.none_roi_delta_qp <= ESP_H264_QP_MAX) && (cfg.none_roi_delta_qp >= -ESP_H264_QP_MAX), ESP_H264_ERR_ARG, TAG, "none_roi_delta_qp error ");
    ESP_H264_RET_ON_FALSE(handle->cfg_roi, ESP_H264_ERR_UNSUPPORTED, TAG, "`cfg_roi` is not supported yet");
    return handle->cfg_roi(handle, cfg);
}

esp_h264_err_t esp_h264_enc_hw_get_roi_cfg_info(esp_h264_enc_param_hw_handle_t handle, esp_h264_enc_roi_cfg_t *out_cfg)
{
    ESP_H264_RET_ON_FALSE(handle, ESP_H264_ERR_ARG, TAG, "Invalid h264 parameter");
    ESP_H264_RET_ON_FALSE(out_cfg, ESP_H264_ERR_ARG, TAG, "The out ROI configure pointer is NULL");
    ESP_H264_RET_ON_FALSE(handle->get_roi_cfg_info, ESP_H264_ERR_UNSUPPORTED, TAG, "`get_roi_cfg_info` is not supported yet");
    return handle->get_roi_cfg_info(handle, out_cfg);
}

esp_h264_err_t esp_h264_enc_hw_set_roi_region(esp_h264_enc_param_hw_handle_t handle, esp_h264_enc_roi_reg_t roi_reg)
{
    ESP_H264_RET_ON_FALSE(handle, ESP_H264_ERR_ARG, TAG, "Invalid h264 parameter");
    ESP_H264_RET_ON_FALSE(roi_reg.qp <= ESP_H264_QP_MAX, ESP_H264_ERR_ARG, TAG, "ROI region QP is gather than 51");
    ESP_H264_RET_ON_FALSE(handle->set_roi_reg, ESP_H264_ERR_UNSUPPORTED, TAG, "`set_roi_reg` is not supported yet");
    return handle->set_roi_reg(handle, roi_reg);
}

esp_h264_err_t esp_h264_enc_hw_get_roi_region(esp_h264_enc_param_hw_handle_t handle, esp_h264_enc_roi_reg_t *out_roi_reg)
{
    ESP_H264_RET_ON_FALSE(handle, ESP_H264_ERR_ARG, TAG, "Invalid h264 parameter");
    ESP_H264_RET_ON_FALSE(out_roi_reg, ESP_H264_ERR_ARG, TAG, "The out ROI region pointer is NULL");
    ESP_H264_RET_ON_FALSE(handle->get_roi_reg, ESP_H264_ERR_UNSUPPORTED, TAG, "`get_roi_reg` is not supported yet");
    return handle->get_roi_reg(handle, out_roi_reg);
}

esp_h264_err_t esp_h264_enc_hw_cfg_mv(esp_h264_enc_param_hw_handle_t handle, esp_h264_enc_mv_cfg_t cfg)
{
    ESP_H264_RET_ON_FALSE(handle, ESP_H264_ERR_ARG, TAG, "Invalid h264 parameter");
    ESP_H264_RET_ON_FALSE(cfg.mv_mode < ESP_H264_MVM_MODE_INVALID, ESP_H264_ERR_ARG, TAG, "The MV mode is gather than or equal ESP_H264_MVM_MODE_INVALID");
    ESP_H264_RET_ON_FALSE(cfg.mv_fmt < ESP_H264_MVM_FMT_INVALID, ESP_H264_ERR_ARG, TAG, "The MV format is gather than or equal ESP_H264_MVM_FMT_INVALID");
    ESP_H264_RET_ON_FALSE(handle->cfg_mv, ESP_H264_ERR_UNSUPPORTED, TAG, "`cfg_mv` is not supported yet");
    return handle->cfg_mv(handle, cfg);
}

esp_h264_err_t esp_h264_enc_hw_get_mv_cfg_info(esp_h264_enc_param_hw_handle_t handle, esp_h264_enc_mv_cfg_t *out_cfg)
{
    ESP_H264_RET_ON_FALSE(handle, ESP_H264_ERR_ARG, TAG, "Invalid h264 parameter");
    ESP_H264_RET_ON_FALSE(out_cfg, ESP_H264_ERR_ARG, TAG, "The out MV configure pointer is NULL");
    ESP_H264_RET_ON_FALSE(handle->get_mv_cfg_info, ESP_H264_ERR_UNSUPPORTED, TAG, "`get_mv_cfg_info` is not supported yet");
    return handle->get_mv_cfg_info(handle, out_cfg);
}

esp_h264_err_t esp_h264_enc_hw_set_mv_pkt(esp_h264_enc_param_hw_handle_t handle, esp_h264_enc_mvm_pkt_t mv_pkt)
{
    ESP_H264_RET_ON_FALSE(handle, ESP_H264_ERR_ARG, TAG, "Invalid h264 parameter");
    ESP_H264_RET_ON_FALSE(mv_pkt.data, ESP_H264_ERR_ARG, TAG, "The MV packet data is NULL.");
    ESP_H264_RET_ON_FALSE(mv_pkt.len > 0, ESP_H264_ERR_ARG, TAG, "The MV packet length is less than or equal 0.");
    ESP_H264_RET_ON_FALSE(handle->set_mv_pkt, ESP_H264_ERR_UNSUPPORTED, TAG, "`set_mv_pkt` is not supported yet");
    return handle->set_mv_pkt(handle, mv_pkt);
}

esp_h264_err_t esp_h264_enc_hw_get_mv_data_len(esp_h264_enc_param_hw_handle_t handle, uint32_t *out_length)
{
    ESP_H264_RET_ON_FALSE(handle, ESP_H264_ERR_ARG, TAG, "Invalid h264 parameter");
    ESP_H264_RET_ON_FALSE(out_length, ESP_H264_ERR_ARG, TAG, "The out length pointer is NULL");
    ESP_H264_RET_ON_FALSE(handle->get_mv_data_len, ESP_H264_ERR_UNSUPPORTED, TAG, "`get_mv_data_len` is not supported yet");
    return handle->get_mv_data_len(handle, out_length);
}
