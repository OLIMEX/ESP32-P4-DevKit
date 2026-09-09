/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_h264_enc_sw_param.h"
#include "esp_h264_check.h"

static const char *TAG = "H264_ENC.SW.PARAM";

typedef struct {
    esp_h264_enc_param_t  sw_base;
    ISVCEncoder          *pPtrEnc;
} esp_h264_enc_sw_param_t;

/** Basic parameter configure */
static esp_h264_err_t get_res(esp_h264_enc_param_t *handle, esp_h264_resolution_t *res)
{
    esp_h264_enc_sw_param_t *param = __containerof(handle, esp_h264_enc_sw_param_t, sw_base);
    ISVCEncoder *pPtrEnc = param->pPtrEnc;
    SEncParamBase base_param;
    (*pPtrEnc)->GetOption(pPtrEnc, ENCODER_OPTION_SVC_ENCODE_PARAM_BASE, &base_param);
    res->width = base_param.iPicWidth;
    res->height = base_param.iPicHeight;
    return ESP_H264_ERR_OK;
}

static esp_h264_err_t set_fps(esp_h264_enc_param_t *handle, uint8_t fps)
{
    esp_h264_enc_sw_param_t *param = __containerof(handle, esp_h264_enc_sw_param_t, sw_base);
    ISVCEncoder *pPtrEnc = param->pPtrEnc;
    float fps_tmp = fps;
    (*pPtrEnc)->SetOption(pPtrEnc, ENCODER_OPTION_FRAME_RATE, &fps_tmp);
    return ESP_H264_ERR_OK;
}

static esp_h264_err_t get_fps(esp_h264_enc_param_t *handle, uint8_t *fps)
{
    esp_h264_enc_sw_param_t *param = __containerof(handle, esp_h264_enc_sw_param_t, sw_base);
    ISVCEncoder *pPtrEnc = param->pPtrEnc;
    float fps_tmp;
    (*pPtrEnc)->GetOption(pPtrEnc, ENCODER_OPTION_FRAME_RATE, &fps_tmp);
    *fps = fps_tmp;
    return ESP_H264_ERR_OK;
}

static esp_h264_err_t set_gop(esp_h264_enc_param_t *handle, uint8_t gop)
{
    esp_h264_enc_sw_param_t *param = __containerof(handle, esp_h264_enc_sw_param_t, sw_base);
    ISVCEncoder *pPtrEnc = param->pPtrEnc;
    int32_t gop_tmp = gop;
    (*pPtrEnc)->SetOption(pPtrEnc, ENCODER_OPTION_IDR_INTERVAL, &gop_tmp);
    return ESP_H264_ERR_OK;
}

static esp_h264_err_t get_gop(esp_h264_enc_param_t *handle, uint8_t *gop)
{
    esp_h264_enc_sw_param_t *param = __containerof(handle, esp_h264_enc_sw_param_t, sw_base);
    ISVCEncoder *pPtrEnc = param->pPtrEnc;
    int32_t gop_tmp = 0;
    (*pPtrEnc)->GetOption(pPtrEnc, ENCODER_OPTION_IDR_INTERVAL, &gop_tmp);
    *gop = gop_tmp;
    return ESP_H264_ERR_OK;
}

static esp_h264_err_t set_bitrate(esp_h264_enc_param_t *handle, uint32_t bitrate)
{
    esp_h264_enc_sw_param_t *param = __containerof(handle, esp_h264_enc_sw_param_t, sw_base);
    ISVCEncoder *pPtrEnc = param->pPtrEnc;
    SBitrateInfo info;
    info.iLayer = SPATIAL_LAYER_ALL;
    (*pPtrEnc)->GetOption(pPtrEnc, ENCODER_OPTION_MAX_BITRATE, &info);
    (*pPtrEnc)->GetOption(pPtrEnc, ENCODER_OPTION_BITRATE, &info);
    info.iBitrate = bitrate;
    info.iLayer = SPATIAL_LAYER_ALL;
    if (info.iBitrate < bitrate) {
        (*pPtrEnc)->SetOption(pPtrEnc, ENCODER_OPTION_MAX_BITRATE, &info);
        info.iLayer = SPATIAL_LAYER_0;
        (*pPtrEnc)->SetOption(pPtrEnc, ENCODER_OPTION_MAX_BITRATE, &info);
        (*pPtrEnc)->SetOption(pPtrEnc, ENCODER_OPTION_BITRATE, &info);
        info.iLayer = SPATIAL_LAYER_ALL;
        (*pPtrEnc)->SetOption(pPtrEnc, ENCODER_OPTION_BITRATE, &info);
    } else {
        (*pPtrEnc)->SetOption(pPtrEnc, ENCODER_OPTION_BITRATE, &info);
    }
    return ESP_H264_ERR_OK;
}

static esp_h264_err_t get_bitrate(esp_h264_enc_param_t *handle, uint32_t *bitrate)
{
    esp_h264_enc_sw_param_t *param = __containerof(handle, esp_h264_enc_sw_param_t, sw_base);
    ISVCEncoder *pPtrEnc = param->pPtrEnc;
    SBitrateInfo info = {
        .iLayer = SPATIAL_LAYER_ALL,
    };
    (*pPtrEnc)->GetOption(pPtrEnc, ENCODER_OPTION_BITRATE, &info);
    *bitrate = info.iBitrate;
    return ESP_H264_ERR_OK;
}

esp_h264_err_t esp_h264_enc_sw_del_param(esp_h264_enc_param_t *handle)
{
    if (handle) {
        esp_h264_enc_sw_param_t *param = __containerof(handle, esp_h264_enc_sw_param_t, sw_base);
        esp_h264_free(param);
    }
    return ESP_H264_ERR_OK;
}

esp_h264_err_t esp_h264_enc_sw_new_param(esp_h264_enc_sw_param_cfg_t *cfg, esp_h264_enc_param_t **handle)
{
    /* Parameter check */
    ESP_H264_RET_ON_FALSE(cfg && handle, ESP_H264_ERR_ARG, TAG, "Invalid parameter");

    *handle = NULL;
    esp_h264_err_t ret = ESP_H264_ERR_OK;
    /** Create a new parameter handle */
    uint32_t actual_size;
    esp_h264_enc_sw_param_t *param = (esp_h264_enc_sw_param_t *)esp_h264_calloc_prefer(1, sizeof(esp_h264_enc_sw_param_t), &actual_size, ESP_H264_MEM_SPIRAM, ESP_H264_MEM_INTERNAL);
    ESP_H264_RET_ON_FALSE(param, ESP_H264_ERR_ARG, TAG, "No memory for handle");

    /* Parameter initalization */
    param->pPtrEnc = cfg->device;

    /** Encoder handle configure */
    param->sw_base.get_res = get_res;
    param->sw_base.set_fps = set_fps;
    param->sw_base.get_fps = get_fps;
    param->sw_base.set_gop = set_gop;
    param->sw_base.get_gop = get_gop;
    param->sw_base.set_bitrate = set_bitrate;
    param->sw_base.get_bitrate = get_bitrate;

    *handle = &param->sw_base;
    return ret;
}
