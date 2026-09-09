/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_h264_enc_single.h"
#include "h264_color_convert.h"
#include "esp_h264_enc_sw_param.h"
#include "esp_h264_enc_single_sw.h"

static const char *TAG = "H264_ENC.SW";

typedef struct esp_h264_enc_sw_handle {
    esp_h264_enc_t        base;
    esp_h264_enc_param_t *param_hd;
    uint8_t               gop;
    esp_h264_raw_format_t pic_type;
    uint8_t              *yuv_cache;
    SSourcePicture        src_pic;
    ISVCEncoder          *pPtrEnc;
    convert_color         cc;
    int                   wxh;
    int                   wxh_q;
} esp_h264_enc_sw_handle_t;

static void fill_enc_param(SEncParamExt *sParam, const esp_h264_enc_cfg_sw_t *cfg)
{
    /* Test for temporal, spatial, SNR scalability */
    sParam->iUsageType = CAMERA_VIDEO_REAL_TIME;
    sParam->fMaxFrameRate = cfg->fps;         // input frame rate
    sParam->iPicWidth = cfg->res.width;       // width of picture in samples
    sParam->iPicHeight = cfg->res.height;     // height of picture in samples
    sParam->iTargetBitrate = cfg->rc.bitrate; // target bitrate desired

    sParam->iTemporalLayerNum = SPATIAL_LAYER_0; // layer number at temporal level
    sParam->iSpatialLayerNum = SPATIAL_LAYER_0;  // layer number at spatial level

    sParam->iComplexityMode = LOW_COMPLEXITY;
    sParam->uiIntraPeriod = cfg->gop;
    sParam->iNumRefFrame = 1;
    sParam->eSpsPpsIdStrategy = CONSTANT_ID;
    sParam->bPrefixNalAddingCtrl = false;
    sParam->bEnableSSEI = false;
    sParam->bSimulcastAVC = false;
    sParam->iEntropyCodingModeFlag = 0;

    /*RC */
    sParam->bEnableFrameSkip = false;
    sParam->iRCMode = RC_QUALITY_MODE; //  rc mode control
    sParam->iMaxBitrate = cfg->rc.bitrate;
    sParam->iMaxQp = cfg->rc.qp_max;
    sParam->iMinQp = cfg->rc.qp_min;

    /*LTR settings*/
    sParam->bEnableLongTermReference = false; // long term reference control

    /* multi-thread settings*/
    sParam->iMultipleThreadIdc = 1;

    /* Deblocking loop filter */
    sParam->iLoopFilterDisableIdc = 1;
    /*pre-processing feature*/
    sParam->bEnableDenoise = false;
    sParam->bEnableAdaptiveQuant = true;
    sParam->bEnableBackgroundDetection = false;
    sParam->bEnableFrameCroppingFlag = (cfg->res.width % 16 != 0) || (cfg->res.height % 16 != 0);
    sParam->bEnableSceneChangeDetect = false;

    sParam->sSpatialLayers[0].uiProfileIdc = PRO_BASELINE;
    sParam->sSpatialLayers[0].iVideoWidth = cfg->res.width;
    sParam->sSpatialLayers[0].iVideoHeight = cfg->res.height;
    sParam->sSpatialLayers[0].fFrameRate = sParam->fMaxFrameRate;
    sParam->sSpatialLayers[0].iSpatialBitrate = sParam->iTargetBitrate;
    sParam->sSpatialLayers[0].iMaxSpatialBitrate = sParam->iMaxBitrate;
    sParam->sSpatialLayers[0].sSliceArgument.uiSliceMode = SM_SINGLE_SLICE;
    sParam->sSpatialLayers[0].iDLayerQp = (sParam->iMaxQp + sParam->iMinQp) >> 1;
}

static esp_h264_err_t esp_h264_enc_hw_res_check(int width, int height)
{
    if ((width > ESP_H264_SW_MIN_WIDTH)
            && (height > ESP_H264_SW_MIN_HEIGHT)) {
        return ESP_H264_ERR_OK;
    }
    return ESP_H264_ERR_FAIL;
}

static esp_h264_err_t h264_sw_enc_process(esp_h264_enc_sw_handle_t *sw_hd, esp_h264_enc_in_frame_t *in_frame, esp_h264_enc_out_frame_t *out_frame)
{
    if (sw_hd->pic_type == ESP_H264_RAW_FMT_I420) {
        sw_hd->src_pic.pData[0] = in_frame->raw_data.buffer;
    } else {
        sw_hd->cc(sw_hd->src_pic.iPicHeight, sw_hd->src_pic.iPicWidth, in_frame->raw_data.buffer, sw_hd->yuv_cache);
        sw_hd->src_pic.pData[0] = sw_hd->yuv_cache;
    }
    out_frame->length = 0;
    sw_hd->src_pic.pData[1] = sw_hd->src_pic.pData[0] + sw_hd->wxh;
    sw_hd->src_pic.pData[2] = sw_hd->src_pic.pData[1] + sw_hd->wxh_q;
    sw_hd->src_pic.uiTimeStamp = in_frame->pts;
    SFrameBSInfo sFbi;
    sFbi.iFrameSizeInBytes = out_frame->raw_data.len;
    sFbi.sLayerInfo[0].pBsBuf = out_frame->raw_data.buffer;
    int iEncFrames = (*(sw_hd->pPtrEnc))->EncodeFrame(sw_hd->pPtrEnc, &sw_hd->src_pic, &sFbi);
    switch (iEncFrames) {
    case cmResultSuccess:
        break;
    case cmUnsupportedData:
    case cmInitParaError:
        return ESP_H264_ERR_ARG;
    case cmMallocMemeError:
        return ESP_H264_ERR_MEM;
    default:
        return ESP_H264_ERR_FAIL;
    }
    out_frame->length = (uint32_t)sFbi.iFrameSizeInBytes;
    out_frame->pts = (uint32_t)sFbi.uiTimeStamp;
    out_frame->dts = in_frame->pts;
    return ESP_H264_ERR_OK;
}

static esp_h264_err_t enc_process(esp_h264_enc_handle_t enc, esp_h264_enc_in_frame_t *in_frame, esp_h264_enc_out_frame_t *out_frame)
{
    esp_h264_enc_sw_handle_t *sw_hd = __containerof(enc, esp_h264_enc_sw_handle_t, base);
    esp_h264_err_t ret = ESP_H264_ERR_OK;
    ret = h264_sw_enc_process(sw_hd, in_frame, out_frame);
    return ret;
}

static esp_h264_err_t enc_close(esp_h264_enc_handle_t enc)
{
    return ESP_H264_ERR_OK;
}

static esp_h264_err_t enc_open(esp_h264_enc_handle_t enc)
{
    esp_h264_enc_sw_handle_t *sw_hd = __containerof(enc, esp_h264_enc_sw_handle_t, base);
    sw_hd->src_pic.iColorFormat = videoFormatI420;
    sw_hd->src_pic.uiTimeStamp = 0;
    sw_hd->wxh = sw_hd->src_pic.iPicWidth * sw_hd->src_pic.iPicHeight;
    sw_hd->wxh_q = sw_hd->wxh >> 2;
    return ESP_H264_ERR_OK;
}

static esp_h264_err_t enc_del(esp_h264_enc_handle_t enc)
{
    if (enc) {
        esp_h264_enc_sw_handle_t *sw_hd = __containerof(enc, esp_h264_enc_sw_handle_t, base);
        if (sw_hd->pPtrEnc) {
            WelsDestroySVCEncoder(sw_hd->pPtrEnc);
        }
        if (sw_hd->yuv_cache) {
            esp_h264_free(sw_hd->yuv_cache);
            sw_hd->yuv_cache = NULL;
        }
        enc_close(enc);
        esp_h264_enc_sw_del_param(sw_hd->param_hd);
        esp_h264_free(sw_hd);
    }
    return ESP_H264_ERR_OK;
}

esp_h264_err_t esp_h264_enc_sw_new(const esp_h264_enc_cfg_sw_t *cfg, esp_h264_enc_handle_t *out_enc)
{
    /* Parameter check */
    ESP_H264_RET_ON_FALSE(cfg && out_enc, ESP_H264_ERR_ARG, TAG, "Invalid h264 configure and handle parameter");
    ESP_H264_RET_ON_FALSE((cfg->pic_type == ESP_H264_RAW_FMT_YUYV) || (cfg->pic_type == ESP_H264_RAW_FMT_I420), ESP_H264_ERR_ARG, TAG, "Un-supported h264 picture type parameter");
    ESP_H264_RET_ON_FALSE((cfg->rc.qp_max >= cfg->rc.qp_min) && (cfg->rc.qp_max <= ESP_H264_QP_MAX), ESP_H264_ERR_ARG, TAG, "Invalid h264 QP parameter");
    ESP_H264_RET_ON_FALSE((esp_h264_enc_hw_res_check(cfg->res.width, cfg->res.height) == ESP_H264_ERR_OK), ESP_H264_ERR_ARG, TAG, "Invalid h264 resolution parameter");
    ESP_H264_RET_ON_FALSE((cfg->fps > 0) && (cfg->gop > 0), ESP_H264_ERR_ARG, TAG, "Invalid h264 FPS and GOP parameter");

    *out_enc = NULL;
    ESP_H264_LOGI(TAG, "openh264 version: %s ", esp_openh264_get_version());
    /** Create encoder handle */
    uint32_t actual_size;
    esp_h264_enc_sw_handle_t *sw_hd = (esp_h264_enc_sw_handle_t *)esp_h264_calloc_prefer(1, sizeof(esp_h264_enc_sw_handle_t), &actual_size, ESP_H264_MEM_SPIRAM, ESP_H264_MEM_INTERNAL);
    ESP_H264_RET_ON_FALSE(sw_hd != NULL, ESP_H264_ERR_MEM, TAG, "No memory for the handle");
    WelsCreateSVCEncoder(&sw_hd->pPtrEnc);

    /* Parameter initalization */
    esp_h264_err_t ret = ESP_H264_ERR_OK;
    esp_h264_enc_sw_param_cfg_t param_cfg = {
        .device = sw_hd->pPtrEnc,
    };
    SEncParamExt sSvcParam;
    (*(sw_hd->pPtrEnc))->GetDefaultParams(sw_hd->pPtrEnc, &sSvcParam);
    fill_enc_param(&sSvcParam, cfg);
    sw_hd->pic_type = cfg->pic_type;
    int iEncFrames = (*(sw_hd->pPtrEnc))->InitializeExt(sw_hd->pPtrEnc, &sSvcParam);
    switch (iEncFrames) {
    case cmResultSuccess:
        break;
    case cmUnsupportedData:
    case cmInitParaError:
        ret = ESP_H264_ERR_ARG;
        goto __exit__;
    case cmMallocMemeError:
        ret = ESP_H264_ERR_MEM;
        goto __exit__;
    default:
        ret = ESP_H264_ERR_FAIL;
        goto __exit__;
    }
    if (sw_hd->pic_type != ESP_H264_RAW_FMT_I420) {
        sw_hd->yuv_cache = (uint8_t *)esp_h264_calloc_prefer(1, cfg->res.height * cfg->res.width * 1.5, &actual_size, ESP_H264_MEM_SPIRAM, ESP_H264_MEM_INTERNAL);
        ESP_H264_GOTO_ON_FALSE(sw_hd->yuv_cache, ESP_H264_ERR_MEM, __exit__, TAG, "No memory for yuv cache");
        sw_hd->cc = yuyv2iyuv;
#ifdef HAVE_ESP32S3
        if (cfg->res.width % 32 == 0
                && cfg->res.height % 2 == 0) {
            sw_hd->cc = yuyv2iyuv_esp32s3;
        }
#endif
    }
    sw_hd->src_pic.iPicWidth = cfg->res.width;
    sw_hd->src_pic.iPicHeight = cfg->res.height;
    sw_hd->src_pic.iStride[0] = cfg->res.width;
    sw_hd->src_pic.iStride[1] = (cfg->res.width >> 1);
    sw_hd->src_pic.iStride[2] = (cfg->res.width >> 1);

    /** Create a new parameter handle */
    ret = esp_h264_enc_sw_new_param(&param_cfg, &sw_hd->param_hd);
    ESP_H264_GOTO_ON_FALSE(ret == ESP_H264_ERR_OK, ret, __exit__, TAG, "No memory for param0 handle");

    /** Encoder handle configure */
    sw_hd->gop = cfg->gop;
    sw_hd->base.open = enc_open;
    sw_hd->base.process = enc_process;
    sw_hd->base.close = enc_close;
    sw_hd->base.del = enc_del;
    *out_enc = &sw_hd->base;
    return ret;
__exit__:
    /** Delete the encoder handle */
    enc_del(&sw_hd->base);
    return ret;
}

esp_h264_err_t esp_h264_enc_sw_get_param_hd(esp_h264_enc_handle_t enc, esp_h264_enc_param_sw_handle_t *out_param)
{
    if (enc && out_param) {
        esp_h264_enc_sw_handle_t *sw_hd = __containerof(enc, esp_h264_enc_sw_handle_t, base);
        *out_param = sw_hd->param_hd;
        return ESP_H264_ERR_OK;
    }
    return ESP_H264_ERR_ARG;
}
