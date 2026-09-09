/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_h264_enc_dual.h"
#include "esp_h264_check.h"

static const char *TAG = "H264_ENC.IF.DUAL";

esp_h264_err_t esp_h264_enc_dual_open(esp_h264_enc_dual_handle_t enc)
{
    ESP_H264_RET_ON_FALSE(enc, ESP_H264_ERR_ARG, TAG, "Invalid h264 handle");
    ESP_H264_RET_ON_FALSE(enc->open, ESP_H264_ERR_UNSUPPORTED, TAG, "Open function is not supported yet");
    return enc->open(enc);
}

esp_h264_err_t esp_h264_enc_dual_process(esp_h264_enc_dual_handle_t enc, esp_h264_enc_in_frame_t *in_frame[2], esp_h264_enc_out_frame_t *out_frame[2])
{
    ESP_H264_RET_ON_FALSE(enc && in_frame && out_frame, ESP_H264_ERR_ARG, TAG, "Invalid h264 handle");
    ESP_H264_RET_ON_FALSE(in_frame[0] && in_frame[1], ESP_H264_ERR_ARG, TAG, "The input frame pointer is NULL.");
    ESP_H264_RET_ON_FALSE(in_frame[0]->raw_data.buffer && in_frame[1]->raw_data.buffer, ESP_H264_ERR_ARG, TAG, "The buffer pointer of input frame is NULL.");
    ESP_H264_RET_ON_FALSE(out_frame[0] && out_frame[1], ESP_H264_ERR_ARG, TAG, "The output frame pointer is NULL.");
    ESP_H264_RET_ON_FALSE(out_frame[0]->raw_data.buffer && out_frame[1]->raw_data.buffer, ESP_H264_ERR_ARG, TAG, "The buffer pointer of output frame is NULL.");
    ESP_H264_RET_ON_FALSE(enc->process, ESP_H264_ERR_UNSUPPORTED, TAG, "Process function is not supported yet");
    return enc->process(enc, in_frame, out_frame);
}

esp_h264_err_t esp_h264_enc_dual_close(esp_h264_enc_dual_handle_t enc)
{
    ESP_H264_RET_ON_FALSE(enc, ESP_H264_ERR_ARG, TAG, "Invalid h264 handle");
    return enc->close(enc);
}

esp_h264_err_t esp_h264_enc_dual_del(esp_h264_enc_dual_handle_t enc)
{
    ESP_H264_RET_ON_FALSE(enc, ESP_H264_ERR_ARG, TAG, "Invalid h264 handle");
    ESP_H264_RET_ON_FALSE(enc->del, ESP_H264_ERR_UNSUPPORTED, TAG, "Delete function is not supported yet");
    return enc->del(enc);
}
