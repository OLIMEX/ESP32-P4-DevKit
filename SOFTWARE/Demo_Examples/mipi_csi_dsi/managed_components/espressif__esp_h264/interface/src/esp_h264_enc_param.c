/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_h264_enc_param.h"
#include "esp_h264_check.h"

static const char *TAG = "H264_ENC.IF.PARAM";

esp_h264_err_t esp_h264_enc_get_resolution(esp_h264_enc_param_handle_t handle, esp_h264_resolution_t *out_res)
{
    ESP_H264_RET_ON_FALSE(handle, ESP_H264_ERR_ARG, TAG, "Invalid h264 parameter");
    ESP_H264_RET_ON_FALSE(out_res, ESP_H264_ERR_ARG, TAG, "The out resolution pointer is NULL");
    ESP_H264_RET_ON_FALSE(handle->get_res, ESP_H264_ERR_UNSUPPORTED, TAG, "`get_res` is not supported yet");
    return handle->get_res(handle, out_res);
}

esp_h264_err_t esp_h264_enc_set_fps(esp_h264_enc_param_handle_t handle, uint8_t fps)
{
    ESP_H264_RET_ON_FALSE(handle, ESP_H264_ERR_ARG, TAG, "Invalid h264 parameter");
    ESP_H264_RET_ON_FALSE(fps > 0, ESP_H264_ERR_ARG, TAG, "FPS is 0.");
    ESP_H264_RET_ON_FALSE(handle->set_fps, ESP_H264_ERR_UNSUPPORTED, TAG, "`set_fps` is not supported yet");
    return handle->set_fps(handle, fps);
}

esp_h264_err_t esp_h264_enc_get_fps(esp_h264_enc_param_handle_t handle, uint8_t *out_fps)
{
    ESP_H264_RET_ON_FALSE(handle, ESP_H264_ERR_ARG, TAG, "Invalid h264 parameter");
    ESP_H264_RET_ON_FALSE(out_fps, ESP_H264_ERR_ARG, TAG, "The out FPS pointer is NULL.");
    ESP_H264_RET_ON_FALSE(handle->get_fps, ESP_H264_ERR_UNSUPPORTED, TAG, "`get_fps` is not supported yet");
    return handle->get_fps(handle, out_fps);
}

esp_h264_err_t esp_h264_enc_set_gop(esp_h264_enc_param_handle_t handle, uint8_t gop)
{
    ESP_H264_RET_ON_FALSE(handle, ESP_H264_ERR_ARG, TAG, "Invalid h264 parameter");
    ESP_H264_RET_ON_FALSE(gop > 0, ESP_H264_ERR_ARG, TAG, "GOP is 0.");
    ESP_H264_RET_ON_FALSE(handle->set_gop, ESP_H264_ERR_UNSUPPORTED, TAG, "`set_gop` is not supported yet");
    return handle->set_gop(handle, gop);
}

esp_h264_err_t esp_h264_enc_get_gop(esp_h264_enc_param_handle_t handle, uint8_t *out_gop)
{
    ESP_H264_RET_ON_FALSE(handle, ESP_H264_ERR_ARG, TAG, "Invalid h264 parameter");
    ESP_H264_RET_ON_FALSE(out_gop, ESP_H264_ERR_ARG, TAG, "The out GOP pointer is NULL.");
    ESP_H264_RET_ON_FALSE(handle->get_gop, ESP_H264_ERR_UNSUPPORTED, TAG, "`get_gop` is not supported yet");
    return handle->get_gop(handle, out_gop);
}

esp_h264_err_t esp_h264_enc_set_bitrate(esp_h264_enc_param_handle_t handle, uint32_t bitrate)
{
    ESP_H264_RET_ON_FALSE(handle, ESP_H264_ERR_ARG, TAG, "Invalid h264 parameter");
    ESP_H264_RET_ON_FALSE(handle->set_bitrate, ESP_H264_ERR_UNSUPPORTED, TAG, "`set_bitrate` is not supported yet");
    return handle->set_bitrate(handle, bitrate);
}

esp_h264_err_t esp_h264_enc_get_bitrate(esp_h264_enc_param_handle_t handle, uint32_t *out_bitrate)
{
    ESP_H264_RET_ON_FALSE(handle && out_bitrate, ESP_H264_ERR_ARG, TAG, "Invalid h264 parameter");
    ESP_H264_RET_ON_FALSE(out_bitrate, ESP_H264_ERR_ARG, TAG, "The out bitrate pointer is NULL.");
    ESP_H264_RET_ON_FALSE(handle->get_bitrate, ESP_H264_ERR_UNSUPPORTED, TAG, "`get_bitrate` is not supported yet");
    return handle->get_bitrate(handle, out_bitrate);
}
