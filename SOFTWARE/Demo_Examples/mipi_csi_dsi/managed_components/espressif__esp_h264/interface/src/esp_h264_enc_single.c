/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_h264_enc_single.h"
#include "esp_h264_check.h"

static const char *TAG = "H264_ENC.IF.SW";

esp_h264_err_t esp_h264_enc_open(esp_h264_enc_handle_t enc)
{
    ESP_H264_RET_ON_FALSE(enc, ESP_H264_ERR_ARG, TAG, "Invalid h264 handle");
    ESP_H264_RET_ON_FALSE(enc->open, ESP_H264_ERR_UNSUPPORTED, TAG, "Open function is not supported yet");
    return enc->open(enc);
}

esp_h264_err_t esp_h264_enc_process(esp_h264_enc_handle_t enc, esp_h264_enc_in_frame_t *in_frame, esp_h264_enc_out_frame_t *out_frame)
{
    ESP_H264_RET_ON_FALSE(enc && in_frame && out_frame, ESP_H264_ERR_ARG, TAG, "Invalid h264 handle");
    ESP_H264_RET_ON_FALSE(in_frame->raw_data.buffer, ESP_H264_ERR_ARG, TAG, "The buffer pointer of input frame is NULL.");
    ESP_H264_RET_ON_FALSE(out_frame->raw_data.buffer, ESP_H264_ERR_ARG, TAG, "The buffer pointer of output frame is NULL.");
    ESP_H264_RET_ON_FALSE(enc->process, ESP_H264_ERR_UNSUPPORTED, TAG, "Process function is not supported yet");
    return enc->process(enc, in_frame, out_frame);
}

esp_h264_err_t esp_h264_enc_close(esp_h264_enc_handle_t enc)
{
    ESP_H264_RET_ON_FALSE(enc, ESP_H264_ERR_ARG, TAG, "Invalid h264 handle");
    ESP_H264_RET_ON_FALSE(enc->close, ESP_H264_ERR_UNSUPPORTED, TAG, "Close function is not supported yet");
    return enc->close(enc);
}

esp_h264_err_t esp_h264_enc_del(esp_h264_enc_handle_t enc)
{
    ESP_H264_RET_ON_FALSE(enc, ESP_H264_ERR_ARG, TAG, "Invalid h264 handle");
    ESP_H264_RET_ON_FALSE(enc->del, ESP_H264_ERR_UNSUPPORTED, TAG, "Delete function is not supported yet");
    return enc->del(enc);
}
