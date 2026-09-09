/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_h264_hw_enc_test.h"
#include "esp_h264_alloc.h"
#include "h264_io.h"

static int write_mvm(esp_h264_enc_mvm_pkt_t *mv_pkt, uint32_t length)
{
    return 1;
}

esp_h264_err_t single_hw_enc_process(esp_h264_enc_cfg_hw_t cfg)
{
    esp_h264_enc_in_frame_t in_frame = {0};
    esp_h264_enc_out_frame_t out_frame = {0};
    esp_h264_err_t ret = ESP_H264_ERR_OK;
    esp_h264_enc_handle_t enc = NULL;
    uint16_t width = ((cfg.res.width + 15) >> 4 << 4);
    uint16_t height = ((cfg.res.height + 15) >> 4 << 4);
    in_frame.raw_data.len = ( width * height + (width * height >> 1));
    in_frame.raw_data.buffer = esp_h264_aligned_calloc(16, 1, in_frame.raw_data.len, &in_frame.raw_data.len, ESP_H264_MEM_INTERNAL);
    if (!in_frame.raw_data.buffer) {
        printf("mem allocation failed.line %d %d %d %d\n", width,  height, (int)in_frame.raw_data.len, __LINE__);
        goto _exit_;
    }
    out_frame.raw_data.len = in_frame.raw_data.len;
    out_frame.raw_data.buffer = esp_h264_aligned_calloc(16, 1, out_frame.raw_data.len, &out_frame.raw_data.len, ESP_H264_MEM_INTERNAL);
    if (!out_frame.raw_data.buffer) {
        printf("mem allocation failed.line %d \n", __LINE__);
        goto _exit_;
    }
    ret = esp_h264_enc_hw_new(&cfg, &enc);
    if (ret != ESP_H264_ERR_OK) {
        printf("new failed. line %d \n", __LINE__);
        goto _exit_;
    }
    ret = esp_h264_enc_open(enc);
    if (ret != ESP_H264_ERR_OK) {
        printf("open failed .line %d \n", __LINE__);
        goto _exit_;
    }
    while (1) {
        int ret_w = read_enc_cb_420(&in_frame, cfg.res.width, cfg.res.height);
        if (ret_w <= 0) {
            break;
        }
        ret = esp_h264_enc_process(enc, &in_frame, &out_frame);
        if (ret != ESP_H264_ERR_OK) {
            printf("process failed. line %d \n", __LINE__);
            goto _exit_;
        }
        write_enc_cb(&out_frame);
    }
_exit_:
    ret = esp_h264_enc_close(enc);
    ret = esp_h264_enc_del(enc);
    if (in_frame.raw_data.buffer) {
        esp_h264_free(in_frame.raw_data.buffer);
    }
    if (out_frame.raw_data.buffer) {
        esp_h264_free(out_frame.raw_data.buffer);
    }
    return ret;
}

esp_h264_err_t dual_hw_enc_process(esp_h264_enc_cfg_dual_hw_t cfg)
{
    esp_h264_enc_in_frame_t *in_frame[2] = {NULL, NULL};
    esp_h264_enc_out_frame_t *out_frame[2] = {NULL, NULL};
    esp_h264_err_t ret = ESP_H264_ERR_OK;
    esp_h264_enc_dual_handle_t enc = NULL;
    int32_t out_length[2];
    int16_t width[2] = { ((cfg.cfg0.res.width + 15) >> 4 << 4), ((cfg.cfg1.res.width + 15) >> 4 << 4)};
    int16_t height[2] = { ((cfg.cfg0.res.height + 15) >> 4 << 4), ((cfg.cfg1.res.height + 15) >> 4 << 4)};
    out_length[0] = (width[0] * height[0] + (width[0] * height[0] >> 1));
    out_length[1] = (width[1] * height[1] + (width[1] * height[1] >> 1));
    for (int16_t i = 0; i < 2; i++) {
        in_frame[i] = heap_caps_calloc(1, sizeof(esp_h264_enc_in_frame_t), ESP_H264_MEM_INTERNAL);
        if (!in_frame[i]) {
            printf("mem allocation failed.line %d \n", __LINE__);
            goto _exit_dual_;
        }
        out_frame[i] = heap_caps_calloc(1, sizeof(esp_h264_enc_out_frame_t), ESP_H264_MEM_INTERNAL);
        if (!out_frame[i]) {
            printf("mem allocation failed.line %d \n", __LINE__);
            goto _exit_dual_;
        }
        in_frame[i]->raw_data.len = out_length[i];
        in_frame[i]->raw_data.buffer = esp_h264_aligned_calloc(16, 1, in_frame[i]->raw_data.len, &in_frame[i]->raw_data.len, ESP_H264_MEM_INTERNAL);
        if (!in_frame[i]->raw_data.buffer) {
            printf("mem allocation failed.line %d \n", __LINE__);
            goto _exit_dual_;
        }
        out_frame[i]->raw_data.len = out_length[i];
        out_frame[i]->raw_data.buffer = esp_h264_aligned_calloc(16, 1, out_frame[i]->raw_data.len, &out_frame[i]->raw_data.len, ESP_H264_MEM_INTERNAL);
        if (!out_frame[i]->raw_data.buffer) {
            printf("mem allocation failed. line %d \n", __LINE__);
            goto _exit_dual_;
        }
        out_length[i] = out_frame[i]->raw_data.len;
    }

    ret = esp_h264_enc_dual_hw_new(&cfg, &enc);
    if (ret != ESP_H264_ERR_OK) {
        printf("new failed. line %d \n", __LINE__);
        goto _exit_dual_;
    }
    ret = esp_h264_enc_dual_open(enc);
    if (ret != ESP_H264_ERR_OK) {
        printf("open failed .line %d \n", __LINE__);
        goto _exit_dual_;
    }
    while (1) {
        for (int16_t i = 0; i < 2; i++) {
            int ret_w = read_enc_cb_420(in_frame[i], width[i], height[i]);
            if (ret_w <= 0) {
                goto _exit_dual_;
            }
        }
        ret = esp_h264_enc_dual_process(enc, in_frame, out_frame);
        if (ret != ESP_H264_ERR_OK) {
            printf("process failed. line %d \n", __LINE__);
            goto _exit_dual_;
        }
        for (int16_t i = 0; i < 2; i++) {
            write_enc_cb(out_frame[i]);
        }
    }
_exit_dual_:
    ret = esp_h264_enc_dual_close(enc);
    ret = esp_h264_enc_dual_del(enc);
    for (int16_t i = 0; i < 2; i++) {
        if (in_frame[i]) {
            if (in_frame[i]->raw_data.buffer) {
                esp_h264_free(in_frame[i]->raw_data.buffer);
                esp_h264_free(in_frame[i]);
            }
        }
        if (out_frame[i]) {
            if (out_frame[i]->raw_data.buffer) {
                esp_h264_free(out_frame[i]->raw_data.buffer);
                esp_h264_free(out_frame[i]);
            }
        }
    }
    return ret;
}

/** GOP FPS RC */
esp_h264_err_t single_hw_enc_thread_test(esp_h264_enc_cfg_hw_t cfg)
{
    esp_h264_enc_in_frame_t in_frame = {0};
    esp_h264_enc_out_frame_t out_frame = {0};
    esp_h264_err_t ret = ESP_H264_ERR_FAIL;
    esp_h264_enc_handle_t enc = NULL;
    esp_h264_resolution_t res;
    esp_h264_enc_rc_t rc;
    uint8_t gop;
    uint8_t fps;
    esp_h264_enc_param_hw_handle_t param_hd;
    int index_c = 0;

    in_frame.raw_data.len = (cfg.res.width * cfg.res.height + (cfg.res.width * cfg.res.height >> 1));
    in_frame.raw_data.buffer = esp_h264_aligned_calloc(16, 1, in_frame.raw_data.len, &in_frame.raw_data.len, ESP_H264_MEM_SPIRAM);
    if (!in_frame.raw_data.buffer) {
        printf("mem allocation failed.line %d \n", __LINE__);
        goto _exit_;
    }
    out_frame.raw_data.len = (cfg.res.width * cfg.res.height + (cfg.res.width * cfg.res.height >> 1)) / 10;
    out_frame.raw_data.buffer = esp_h264_aligned_calloc(16, 1, out_frame.raw_data.len, &out_frame.raw_data.len, ESP_H264_MEM_SPIRAM);
    if (!out_frame.raw_data.buffer) {
        printf("mem allocation failed.line %d \n", __LINE__);
        goto _exit_;
    }

    ret = esp_h264_enc_hw_new(&cfg, &enc);
    if (ret != ESP_H264_ERR_OK) {
        printf("new failed. line %d \n", __LINE__);
        goto _exit_;
    }

    ret = esp_h264_enc_hw_get_param_hd(enc, &param_hd);
    if (ret != ESP_H264_ERR_OK) {
        printf("esp_h264_enc_hw_get_param_hd error. line %d \n", __LINE__);
        goto _exit_;
    }

    ret = esp_h264_enc_get_resolution(&param_hd->base, &res);
    if ((ret != ESP_H264_ERR_OK)
            || (res.width != cfg.res.width)
            || (res.height != cfg.res.height)) {
        printf("esp_h264_enc_get_resolution failed .line %d \n", __LINE__);
        goto _exit_;
    }

    ret = esp_h264_enc_get_fps(&param_hd->base, &fps);
    if ((ret != ESP_H264_ERR_OK)
            || (fps != cfg.fps)) {
        printf("esp_h264_enc_get_fps failed .line %d \n", __LINE__);
        goto _exit_;
    }

    ret = esp_h264_enc_get_gop(&param_hd->base, &gop);
    if ((ret != ESP_H264_ERR_OK)
            || (gop != cfg.gop)) {
        printf("esp_h264_enc_get_gop failed .line %d \n", __LINE__);
        goto _exit_;
    }

    ret = esp_h264_enc_get_bitrate(&param_hd->base, &rc.bitrate);
    if (ret != ESP_H264_ERR_OK
            || (rc.bitrate != cfg.rc.bitrate)) {
        printf("esp_h264_enc_get_bitrate failed .line %d \n", __LINE__);
        goto _exit_;
    }

    ret = esp_h264_enc_open(enc);
    if (ret != ESP_H264_ERR_OK) {
        printf("open failed .line %d \n", __LINE__);
        goto _exit_;
    }
    while (1) {
        index_c++;
        int ret_w = read_enc_cb_420(&in_frame, cfg.res.width, cfg.res.height);
        if (ret_w <= 0) {
            break;
        }
        ret |= esp_h264_enc_get_resolution(&param_hd->base, &res);
        if ((ret != ESP_H264_ERR_OK)
                || (res.width != cfg.res.width)
                || (res.height != cfg.res.height)) {
            printf("esp_h264_enc_get_resolution failed .line %d \n", __LINE__);
            goto _exit_;
        }

        ret |= esp_h264_enc_get_fps(&param_hd->base, &fps);
        if ((ret != ESP_H264_ERR_OK)
                || (fps != cfg.fps)) {
            printf("esp_h264_enc_get_fps failed .line %d \n", __LINE__);
            goto _exit_;
        }

        ret |= esp_h264_enc_get_gop(&param_hd->base, &gop);
        if ((ret != ESP_H264_ERR_OK)
                || (gop != cfg.gop)) {
            printf("esp_h264_enc_get_gop failed .line %d \n", __LINE__);
            goto _exit_;
        }

        ret |= esp_h264_enc_get_bitrate(&param_hd->base, &rc.bitrate);
        if (ret != ESP_H264_ERR_OK
                || (rc.bitrate != cfg.rc.bitrate)) {
            printf("esp_h264_enc_get_bitrate failed .line %d \n", __LINE__);
            goto _exit_;
        }

        cfg.fps = index_c + 4;
        ret |= esp_h264_enc_set_fps(&param_hd->base, cfg.fps);
        if (ret != ESP_H264_ERR_OK) {
            printf("esp_h264_enc_set_fps failed .line %d \n", __LINE__);
            goto _exit_;
        }
        cfg.gop = index_c + 3;
        ret |= esp_h264_enc_set_gop(&param_hd->base, cfg.gop);
        if (ret != ESP_H264_ERR_OK) {
            printf("esp_h264_enc_set_gop failed .line %d \n", __LINE__);
            goto _exit_;
        }

        cfg.rc.qp_min = index_c + 3;
        cfg.rc.qp_max = index_c + 10;
        ret |= esp_h264_enc_set_bitrate(&param_hd->base, cfg.rc.bitrate);
        if (ret != ESP_H264_ERR_OK) {
            printf("esp_h264_enc_set_bitrate failed .line %d \n", __LINE__);
            goto _exit_;
        }
        ret |= esp_h264_enc_process(enc, &in_frame, &out_frame);
        if (ret != ESP_H264_ERR_OK) {
            printf("process failed. line %d \n", __LINE__);
            goto _exit_;
        }
        write_enc_cb(&out_frame);
    }
_exit_:
    ret |= esp_h264_enc_close(enc);
    ret |= esp_h264_enc_del(enc);
    if (in_frame.raw_data.buffer) {
        esp_h264_free(in_frame.raw_data.buffer);
    }
    if (out_frame.raw_data.buffer) {
        esp_h264_free(out_frame.raw_data.buffer);
    }
    return ret;
}

esp_h264_err_t dual_hw_enc_thread_test(esp_h264_enc_cfg_dual_hw_t cfg)
{
    esp_h264_err_t ret = ESP_H264_ERR_FAIL;
    int index_c = 0;
    esp_h264_resolution_t res;
    esp_h264_enc_rc_t rc;
    uint8_t gop[2] = { cfg.cfg0.gop, cfg.cfg1.gop };
    uint8_t gop_tmp;
    uint8_t fps;
    esp_h264_enc_param_hw_handle_t param_hd;
    esp_h264_enc_param_hw_handle_t param_hd0;
    esp_h264_enc_param_hw_handle_t param_hd1;
    esp_h264_enc_in_frame_t *in_frame[2] = {NULL, NULL};
    esp_h264_enc_out_frame_t *out_frame[2] = {NULL, NULL};
    esp_h264_enc_dual_handle_t enc = NULL;
    int16_t out_length[2];
    esp_h264_enc_cfg_t base_cfg;
    int16_t width[2] = { cfg.cfg0.res.width, cfg.cfg1.res.width };
    int16_t height[2] = { cfg.cfg0.res.height, cfg.cfg1.res.height };
    out_length[0] = (cfg.cfg0.res.width * cfg.cfg0.res.height + (cfg.cfg0.res.width * cfg.cfg0.res.height >> 1));
    out_length[1] = (cfg.cfg1.res.width * cfg.cfg1.res.height + (cfg.cfg1.res.width * cfg.cfg1.res.height >> 1));

    for (int16_t i = 0; i < 2; i++) {
        in_frame[i] = heap_caps_calloc(1, sizeof(esp_h264_enc_in_frame_t), MALLOC_CAP_INTERNAL);
        if (!in_frame[i]) {
            printf("mem allocation failed.line %d \n", __LINE__);
            goto _exit_dual_;
        }
        out_frame[i] = heap_caps_calloc(1, sizeof(esp_h264_enc_out_frame_t), MALLOC_CAP_INTERNAL);
        if (!out_frame[i]) {
            printf("mem allocation failed.line %d \n", __LINE__);
            goto _exit_dual_;
        }
        in_frame[i]->raw_data.len = out_length[i];
        in_frame[i]->raw_data.buffer = esp_h264_aligned_calloc(16, 1, in_frame[i]->raw_data.len, &in_frame[i]->raw_data.len, ESP_H264_MEM_INTERNAL);
        if (!in_frame[i]->raw_data.buffer) {
            printf("mem allocation failed.line %d \n", __LINE__);
            goto _exit_dual_;
        }
        out_frame[i]->raw_data.len = out_length[i];
        out_frame[i]->raw_data.buffer = esp_h264_aligned_calloc(16, 1, out_frame[i]->raw_data.len, &out_frame[i]->raw_data.len, ESP_H264_MEM_INTERNAL);
        if (!out_frame[i]->raw_data.buffer) {
            printf("mem allocation failed. line %d \n", __LINE__);
            goto _exit_dual_;
        }
        out_length[i] = out_frame[i]->raw_data.len;
    }

    ret = esp_h264_enc_dual_hw_new(&cfg, &enc);
    if (ret != ESP_H264_ERR_OK) {
        printf("new failed. line %d \n", __LINE__);
        goto _exit_dual_;
    }

    ret = esp_h264_enc_dual_hw_get_param_hd0(enc, &param_hd0);
    if (ret != ESP_H264_ERR_OK) {
        printf("esp_h264_enc_hw_get_param_hd error. line %d \n", __LINE__);
        goto _exit_dual_;
    }

    ret = esp_h264_enc_dual_hw_get_param_hd1(enc, &param_hd1);
    if (ret != ESP_H264_ERR_OK) {
        printf("esp_h264_enc_hw_get_param_hd error. line %d \n", __LINE__);
        goto _exit_dual_;
    }

    param_hd = param_hd0;
    ret = esp_h264_enc_dual_open(enc);
    if (ret != ESP_H264_ERR_OK) {
        printf("open failed .line %d \n", __LINE__);
        goto _exit_dual_;
    }
    while (1) {
        index_c++;
        param_hd = index_c % 2 ? param_hd0 : param_hd1;
        memcpy(&base_cfg, index_c % 2 ? &cfg.cfg0 : &cfg.cfg1, sizeof(esp_h264_enc_cfg_t));
        for (int16_t i = 0; i < 2; i++) {
            int ret_w = read_enc_cb_420(in_frame[i], width[i], height[i]);
            if (ret_w <= 0) {
                goto _exit_dual_;
            }
        }

        base_cfg.fps = index_c + 4;
        ret = esp_h264_enc_set_fps(&param_hd->base, base_cfg.fps);
        if (ret != ESP_H264_ERR_OK) {
            printf("esp_h264_enc_set_fps failed .line %d \n", __LINE__);
            goto _exit_dual_;
        }
        gop[index_c % 2] = (index_c + 3);
        ret = esp_h264_enc_set_gop(&param_hd->base, gop[index_c % 2]);
        if (ret != ESP_H264_ERR_OK) {
            printf("esp_h264_enc_set_gop failed .line %d \n", __LINE__);
            goto _exit_dual_;
        }

        base_cfg.rc.qp_min = index_c + 3;
        base_cfg.rc.qp_max = index_c + 10;
        base_cfg.rc.bitrate = base_cfg.res.width * base_cfg.res.height / 20;
        ret = esp_h264_enc_set_bitrate(&param_hd->base, base_cfg.rc.bitrate);
        if (ret != ESP_H264_ERR_OK) {
            printf("esp_h264_enc_set_bitrate failed .line %d \n", __LINE__);
            goto _exit_dual_;
        }
        ret = esp_h264_enc_dual_process(enc, in_frame, out_frame);
        if (ret != ESP_H264_ERR_OK) {
            printf("process failed. line %d \n", __LINE__);
            goto _exit_dual_;
        }
        for (int16_t i = 0; i < 2; i++) {
            write_enc_cb(out_frame[i]);
        }
        ret = esp_h264_enc_get_resolution(&param_hd->base, &res);
        if ((ret != ESP_H264_ERR_OK)
                || (res.width != base_cfg.res.width)
                || (res.height != base_cfg.res.height)) {
            printf("esp_h264_enc_get_resolution failed .line %d \n", __LINE__);
            goto _exit_dual_;
        }

        ret = esp_h264_enc_get_fps(&param_hd->base, &fps);
        if ((ret != ESP_H264_ERR_OK)
                || (fps != base_cfg.fps)) {
            printf("esp_h264_enc_get_fps failed .line %d \n", __LINE__);
            goto _exit_dual_;
        }

        ret = esp_h264_enc_get_gop(&param_hd->base, &gop_tmp);

        if ((ret != ESP_H264_ERR_OK)
                || (gop_tmp != ((gop[0] + gop[1]) >> 1)
                    && gop_tmp != gop[index_c % 2])) {
            printf("esp_h264_enc_get_gop failed . %d %d %d %d line %d \n", index_c, gop[0], gop[1], gop_tmp, __LINE__);
            goto _exit_dual_;
        }
        ret = esp_h264_enc_get_bitrate(&param_hd->base, &rc.bitrate);
        if (ret != ESP_H264_ERR_OK
                || (rc.bitrate != base_cfg.rc.bitrate)) {
            printf("esp_h264_enc_get_bitrate failed .line %d \n", __LINE__);
            printf("bitrate %d %d \n", (int)rc.bitrate, (int)base_cfg.rc.bitrate);
            goto _exit_dual_;
        }
    }
_exit_dual_:
    ret |= esp_h264_enc_dual_close(enc);
    ret |= esp_h264_enc_dual_del(enc);
    for (int16_t i = 0; i < 2; i++) {
        if (in_frame[i]) {
            if (in_frame[i]->raw_data.buffer) {
                esp_h264_free(in_frame[i]->raw_data.buffer);
                esp_h264_free(in_frame[i]);
            }
        }
        if (out_frame[i]) {
            if (out_frame[i]->raw_data.buffer) {
                esp_h264_free(out_frame[i]->raw_data.buffer);
                esp_h264_free(out_frame[i]);
            }
        }
    }
    return ret;
}

/** ROI component*/
esp_h264_err_t single_hw_enc_roi_cfg_test(esp_h264_enc_cfg_hw_t cfg)
{
    esp_h264_enc_in_frame_t in_frame = {0};
    esp_h264_enc_out_frame_t out_frame = {0};
    esp_h264_err_t ret = ESP_H264_ERR_FAIL;
    esp_h264_enc_handle_t enc = NULL;
    esp_h264_enc_param_hw_handle_t param_hd;
    esp_h264_enc_roi_cfg_t roi_cfg = {
        .roi_mode = ESP_H264_ROI_MODE_DELTA_QP,
        .none_roi_delta_qp = 2,
    };
    esp_h264_enc_roi_cfg_t cfg_get = {
        .roi_mode = ESP_H264_ROI_MODE_DELTA_QP,
        .none_roi_delta_qp = -51,
    };
    in_frame.raw_data.len = (cfg.res.width * cfg.res.height + (cfg.res.width * cfg.res.height >> 1));
    in_frame.raw_data.buffer = esp_h264_aligned_calloc(16, 1, in_frame.raw_data.len, &in_frame.raw_data.len, ESP_H264_MEM_INTERNAL);
    if (!in_frame.raw_data.buffer) {
        printf("mem allocation failed.line %d \n", __LINE__);
        goto _exit_;
    }
    out_frame.raw_data.len = (cfg.res.width * cfg.res.height + (cfg.res.width * cfg.res.height >> 1)) / 10;
    out_frame.raw_data.buffer = esp_h264_aligned_calloc(16, 1, out_frame.raw_data.len, &out_frame.raw_data.len, ESP_H264_MEM_INTERNAL);
    if (!out_frame.raw_data.buffer) {
        printf("mem allocation failed.line %d \n", __LINE__);
        goto _exit_;
    }

    ret = esp_h264_enc_hw_new(&cfg, &enc);
    if (ret != ESP_H264_ERR_OK) {
        printf("new failed. line %d \n", __LINE__);
        goto _exit_;
    }

    ret = esp_h264_enc_hw_get_param_hd(enc, &param_hd);
    if (ret != ESP_H264_ERR_OK) {
        ret = ESP_H264_ERR_FAIL;
        printf("esp_h264_enc_hw_get_param_hd error. line %d \n", __LINE__);
        goto _exit_;
    }
    ret = esp_h264_enc_open(enc);
    if (ret != ESP_H264_ERR_OK) {
        printf("open failed .line %d \n", __LINE__);
        goto _exit_;
    }
    for (int8_t i = ESP_H264_ROI_MODE_DISABLE; i < ESP_H264_ROI_MODE_INVALID; i++) {
        roi_cfg.roi_mode = i;
        roi_cfg.none_roi_delta_qp = -26;
        while (1) {
            roi_cfg.none_roi_delta_qp = (roi_cfg.none_roi_delta_qp + 1) % 51;
            ret = esp_h264_enc_hw_cfg_roi(param_hd, roi_cfg);
            if (ret != ESP_H264_ERR_OK) {
                printf("ROI configure error. line %d \n", __LINE__);
                goto _exit_;
            }
            int ret_w = read_enc_cb_420(&in_frame, cfg.res.width, cfg.res.height);
            if (ret_w <= 0) {
                break;
            }
            ret = esp_h264_enc_process(enc, &in_frame, &out_frame);
            if (ret != ESP_H264_ERR_OK) {
                printf("process failed. line %d \n", __LINE__);
                goto _exit_;
            }
            write_enc_cb(&out_frame);
            ret = esp_h264_enc_hw_get_roi_cfg_info(param_hd, &cfg_get);
            if (ret != ESP_H264_ERR_OK
                    || cfg_get.roi_mode != roi_cfg.roi_mode
                    || ((cfg_get.roi_mode != ESP_H264_ROI_MODE_DISABLE)
                        && (cfg_get.none_roi_delta_qp != roi_cfg.none_roi_delta_qp))) {
                printf("ROI process error. %d %d %d line %d \n", cfg_get.roi_mode, cfg_get.none_roi_delta_qp, roi_cfg.none_roi_delta_qp, __LINE__);
                goto _exit_;
            }
        }
    }
_exit_:
    ret |= esp_h264_enc_close(enc);
    ret |= esp_h264_enc_del(enc);
    if (in_frame.raw_data.buffer) {
        esp_h264_free(in_frame.raw_data.buffer);
    }
    if (out_frame.raw_data.buffer) {
        esp_h264_free(out_frame.raw_data.buffer);
    }
    return ret;
}

esp_h264_err_t dual_hw_enc_roi_cfg_test(esp_h264_enc_cfg_dual_hw_t cfg)
{
    esp_h264_enc_in_frame_t *in_frame[2] = {NULL, NULL};
    int index_c = 0;
    esp_h264_enc_out_frame_t *out_frame[2] = {NULL, NULL};
    esp_h264_err_t ret = ESP_H264_ERR_FAIL;
    esp_h264_enc_dual_handle_t enc = NULL;
    int16_t out_length[2];
    esp_h264_enc_param_hw_handle_t param_hd0;
    esp_h264_enc_param_hw_handle_t param_hd1;
    esp_h264_enc_param_hw_handle_t param_hd;
    int16_t width[2] = { cfg.cfg0.res.width, cfg.cfg1.res.width };
    int16_t height[2] = { cfg.cfg0.res.height, cfg.cfg1.res.height };
    out_length[0] = (cfg.cfg0.res.width * cfg.cfg0.res.height + (cfg.cfg0.res.width * cfg.cfg0.res.height >> 1));
    out_length[1] = (cfg.cfg1.res.width * cfg.cfg1.res.height + (cfg.cfg1.res.width * cfg.cfg1.res.height >> 1));
    esp_h264_enc_roi_cfg_t roi_cfg = {
        .roi_mode = ESP_H264_ROI_MODE_DELTA_QP,
        .none_roi_delta_qp = 2,
    };
    esp_h264_enc_roi_cfg_t cfg_get = {
        .roi_mode = ESP_H264_ROI_MODE_DELTA_QP,
        .none_roi_delta_qp = -51,
    };
    for (int16_t i = 0; i < 2; i++) {
        in_frame[i] = heap_caps_calloc(1, sizeof(esp_h264_enc_in_frame_t), MALLOC_CAP_INTERNAL);
        if (!in_frame[i]) {
            printf("mem allocation failed.line %d \n", __LINE__);
            goto _exit_dual_;
        }
        out_frame[i] = heap_caps_calloc(1, sizeof(esp_h264_enc_out_frame_t), MALLOC_CAP_INTERNAL);
        if (!out_frame[i]) {
            printf("mem allocation failed.line %d \n", __LINE__);
            goto _exit_dual_;
        }
        in_frame[i]->raw_data.len = out_length[i];
        in_frame[i]->raw_data.buffer = esp_h264_aligned_calloc(16, 1, in_frame[i]->raw_data.len, &in_frame[i]->raw_data.len, ESP_H264_MEM_INTERNAL);
        if (!in_frame[i]->raw_data.buffer) {
            printf("mem allocation failed.line %d \n", __LINE__);
            goto _exit_dual_;
        }
        out_frame[i]->raw_data.len = out_length[i];
        out_frame[i]->raw_data.buffer = esp_h264_aligned_calloc(16, 1, out_frame[i]->raw_data.len, &out_frame[i]->raw_data.len, ESP_H264_MEM_INTERNAL);
        if (!out_frame[i]->raw_data.buffer) {
            printf("mem allocation failed. line %d \n", __LINE__);
            goto _exit_dual_;
        }
        out_length[i] = out_frame[i]->raw_data.len;
    }

    ret = esp_h264_enc_dual_hw_new(&cfg, &enc);
    if (ret != ESP_H264_ERR_OK) {
        printf("new failed. line %d \n", __LINE__);
        goto _exit_dual_;
    }
    ret = esp_h264_enc_dual_open(enc);
    if (ret != ESP_H264_ERR_OK) {
        printf("open failed .line %d \n", __LINE__);
        goto _exit_dual_;
    }
    ret = esp_h264_enc_dual_hw_get_param_hd0(enc, &param_hd0);
    if (ret != ESP_H264_ERR_OK) {
        printf("esp_h264_enc_hw_get_param_hd error. line %d \n", __LINE__);
        goto _exit_dual_;
    }
    ret = esp_h264_enc_dual_hw_get_param_hd1(enc, &param_hd1);
    if (ret != ESP_H264_ERR_OK) {
        printf("esp_h264_enc_hw_get_param_hd error. line %d \n", __LINE__);
        goto _exit_dual_;
    }
    for (int8_t i = ESP_H264_ROI_MODE_DISABLE; i < ESP_H264_ROI_MODE_INVALID; i++) {
        roi_cfg.roi_mode = i;
        roi_cfg.none_roi_delta_qp = -26;
        while (1) {
            index_c++;
            param_hd = index_c % 2 ? param_hd0 : param_hd1;
            roi_cfg.none_roi_delta_qp = (roi_cfg.none_roi_delta_qp + 1) % 51;
            ret = esp_h264_enc_hw_cfg_roi(param_hd, roi_cfg);
            if (ret != ESP_H264_ERR_OK) {
                printf("ROI configure error. line %d \n", __LINE__);
                goto _exit_dual_;
            }
            for (int16_t i = 0; i < 2; i++) {
                int ret_w = read_enc_cb_420(in_frame[i], width[i], height[i]);
                if (ret_w <= 0) {
                    goto _exit_dual_;
                }
                out_frame[i]->raw_data.len = out_length[i];
            }

            ret = esp_h264_enc_dual_process(enc, in_frame, out_frame);
            if (ret != ESP_H264_ERR_OK) {
                printf("process failed. line %d \n", __LINE__);
                goto _exit_dual_;
            }
            for (int16_t i = 0; i < 2; i++) {
                write_enc_cb(out_frame[i]);
            }
            ret = esp_h264_enc_hw_get_roi_cfg_info(param_hd, &cfg_get);
            if (ret != ESP_H264_ERR_OK
                    || cfg_get.roi_mode != roi_cfg.roi_mode
                    || ((cfg_get.roi_mode != ESP_H264_ROI_MODE_DISABLE)
                        && (cfg_get.none_roi_delta_qp != roi_cfg.none_roi_delta_qp))) {
                printf("ROI process error. %d %d %d line %d \n", cfg_get.roi_mode, cfg_get.none_roi_delta_qp, roi_cfg.none_roi_delta_qp, __LINE__);
                goto _exit_dual_;
            }
        }
    }
_exit_dual_:
    ret |= esp_h264_enc_dual_close(enc);
    ret |= esp_h264_enc_dual_del(enc);
    for (int16_t i = 0; i < 2; i++) {
        if (in_frame[i]) {
            if (in_frame[i]->raw_data.buffer) {
                esp_h264_free(in_frame[i]->raw_data.buffer);
                esp_h264_free(in_frame[i]);
            }
        }
        if (out_frame[i]) {
            if (out_frame[i]->raw_data.buffer) {
                esp_h264_free(out_frame[i]->raw_data.buffer);
                esp_h264_free(out_frame[i]);
            }
        }
    }
    return ret;
}

esp_h264_err_t single_hw_enc_roi_reg_test(esp_h264_enc_cfg_hw_t cfg)
{
    esp_h264_enc_in_frame_t in_frame = {0};
    esp_h264_enc_out_frame_t out_frame = {0};
    esp_h264_err_t ret = ESP_H264_ERR_FAIL;
    esp_h264_enc_handle_t enc = NULL;
    esp_h264_enc_param_hw_handle_t param_hd;
    esp_h264_enc_roi_cfg_t roi_cfg = {
        .roi_mode = ESP_H264_ROI_MODE_DELTA_QP,
        .none_roi_delta_qp = 2,
    };
    int index_c = 0;
    esp_h264_enc_roi_reg_t roi_reg[8] = { 0 };
    esp_h264_enc_roi_reg_t roi_reg1[8] = { 0 };
    in_frame.raw_data.len = (cfg.res.width * cfg.res.height + (cfg.res.width * cfg.res.height >> 1));
    in_frame.raw_data.buffer = esp_h264_aligned_calloc(16, 1, in_frame.raw_data.len, &in_frame.raw_data.len, ESP_H264_MEM_INTERNAL);
    if (!in_frame.raw_data.buffer) {
        printf("mem allocation failed.line %d \n", __LINE__);
        goto _exit_;
    }
    out_frame.raw_data.len = (cfg.res.width * cfg.res.height + (cfg.res.width * cfg.res.height >> 1)) / 10;
    out_frame.raw_data.buffer = esp_h264_aligned_calloc(16, 1, out_frame.raw_data.len, &out_frame.raw_data.len, ESP_H264_MEM_INTERNAL);
    if (!out_frame.raw_data.buffer) {
        printf("mem allocation failed.line %d \n", __LINE__);
        goto _exit_;
    }

    ret = esp_h264_enc_hw_new(&cfg, &enc);
    if (ret != ESP_H264_ERR_OK) {
        printf("new failed. line %d \n", __LINE__);
        goto _exit_;
    }

    ret = esp_h264_enc_hw_get_param_hd(enc, &param_hd);
    if (ret != ESP_H264_ERR_OK) {
        printf("esp_h264_enc_hw_get_param_hd error. line %d \n", __LINE__);
        goto _exit_;
    }
    ret = esp_h264_enc_open(enc);
    if (ret != ESP_H264_ERR_OK) {
        printf("open failed .line %d \n", __LINE__);
        goto _exit_;
    }

    for (int8_t idx = ESP_H264_ROI_MODE_DISABLE; idx < ESP_H264_ROI_MODE_INVALID; idx++) {
        roi_cfg.roi_mode = idx;
        roi_cfg.none_roi_delta_qp = (roi_cfg.none_roi_delta_qp + 1) % 51;
        ret = esp_h264_enc_hw_cfg_roi(param_hd, roi_cfg);
        if (ret != ESP_H264_ERR_OK) {
            printf("ROI configure error. line %d \n", __LINE__);
            goto _exit_;
        }
        while (1) {
            index_c ++;
            roi_reg[0].x = 0;
            roi_reg[0].y = 0;
            roi_reg[0].len_x = 0;
            roi_reg[0].len_y = 0;
            roi_reg[1].x = 0;
            roi_reg[1].y = 0;
            roi_reg[1].len_x = (cfg.res.width + 15) >> 4;
            roi_reg[1].len_y = ((cfg.res.height + 15) >> 4) >> 3;
            roi_reg[2].x = 0;
            roi_reg[2].y = 0;
            roi_reg[2].len_x = ((cfg.res.width + 15) >> 4) >> 3;
            roi_reg[2].len_y = (cfg.res.height + 15) >> 4;
            roi_reg[3].x = 0;
            roi_reg[3].y = ((cfg.res.height + 15) >> 4) * 7 >> 3;
            roi_reg[3].len_x = ((cfg.res.width + 15) >> 4);
            roi_reg[3].len_y = ((cfg.res.height + 15) >> 4) >> 3;
            roi_reg[4].x = ((cfg.res.width + 15) >> 4) * 7 >> 3;
            roi_reg[4].y = 0;
            roi_reg[4].len_x = ((cfg.res.width + 15) >> 4) >> 3;
            roi_reg[4].len_y = (cfg.res.height + 15) >> 4;
            roi_reg[5].x = ((cfg.res.width + 15) >> 4) >> 3;
            roi_reg[5].y = ((cfg.res.height + 15) >> 4) >> 3;
            roi_reg[5].len_x = ((cfg.res.width + 15) >> 4) >> 3;
            roi_reg[5].len_y = ((cfg.res.height + 15) >> 4) >> 3;
            roi_reg[6].x = ((cfg.res.width + 15) >> 4) * 2 >> 3;
            roi_reg[6].y = ((cfg.res.height + 15) >> 4) * 2 >> 3;
            roi_reg[6].len_x = ((cfg.res.width + 15) >> 4) >> 3;
            roi_reg[6].len_y = ((cfg.res.height + 15) >> 4) >> 3;
            roi_reg[7].x = ((cfg.res.width + 15) >> 4) * 3 >> 3;
            roi_reg[7].y = ((cfg.res.height + 15) >> 4) * 3 >> 3;
            roi_reg[7].len_x = ((cfg.res.width + 15) >> 4) * 2 >> 3;
            roi_reg[7].len_y = ((cfg.res.height + 15) >> 4) * 2 >> 3;
            if (idx > ESP_H264_ROI_MODE_DISABLE) {
                for (uint8_t i = 0; i < 8; i++) {
                    roi_reg[i].reg_idx = i;
                    if (roi_cfg.roi_mode == ESP_H264_ROI_MODE_FIX_QP) {
                        roi_reg[i].qp = ((i + index_c - 51) % 51) < 0 ? 0 : ((i + index_c - 51) % 51);
                    } else {
                        roi_reg[i].qp = (i + index_c - 51) % 51;
                    }
                    ret = esp_h264_enc_hw_set_roi_region(param_hd, roi_reg[i]);
                    if (ret != ESP_H264_ERR_OK) {
                        printf("ROI region error. line %d \n", __LINE__);
                        goto _exit_;
                    }
                }
            }

            roi_reg[0].qp = 0;
            int ret_w = read_enc_cb_420(&in_frame, cfg.res.width, cfg.res.height);
            if (ret_w <= 0) {
                break;
            }
            ret = esp_h264_enc_process(enc, &in_frame, &out_frame);
            if (ret != ESP_H264_ERR_OK) {
                printf("process failed. line %d \n", __LINE__);
                goto _exit_;
            }
            write_enc_cb(&out_frame);
            for (size_t i = 0; i < 8; i++) {
                roi_reg1[i].reg_idx = i;
                ret = esp_h264_enc_hw_get_roi_region(param_hd, &roi_reg1[i]);
                if (roi_cfg.roi_mode == ESP_H264_ROI_MODE_DISABLE) {
                    if (ret != ESP_H264_ERR_OK
                            || roi_reg1[i].x != 0
                            || roi_reg1[i].y != 0
                            || roi_reg1[i].len_x != 0
                            || roi_reg1[i].len_y != 0
                            || roi_reg1[i].qp != 0) {
                        goto _exit_;
                    }
                } else {
                    if (ret != ESP_H264_ERR_OK
                            || roi_reg1[i].x != roi_reg[i].x
                            || roi_reg1[i].y != roi_reg[i].y
                            || roi_reg1[i].len_x != roi_reg[i].len_x
                            || roi_reg1[i].len_y != roi_reg[i].len_y
                            || roi_reg1[i].qp != roi_reg[i].qp) {
                        goto _exit_;
                    }
                }
            }
        }
    }
_exit_:
    ret |= esp_h264_enc_close(enc);
    ret |= esp_h264_enc_del(enc);
    if (in_frame.raw_data.buffer) {
        esp_h264_free(in_frame.raw_data.buffer);
    }
    if (out_frame.raw_data.buffer) {
        esp_h264_free(out_frame.raw_data.buffer);
    }
    return ret;
}

esp_h264_err_t dual_hw_enc_roi_reg_test(esp_h264_enc_cfg_dual_hw_t cfg)
{
    esp_h264_enc_in_frame_t *in_frame[2] = {NULL, NULL};
    esp_h264_enc_out_frame_t *out_frame[2] = {NULL, NULL};
    esp_h264_err_t ret = ESP_H264_ERR_FAIL;
    esp_h264_enc_dual_handle_t enc = NULL;
    int16_t out_length[2];
    esp_h264_enc_param_hw_handle_t param_hd0;
    esp_h264_enc_param_hw_handle_t param_hd1;
    esp_h264_enc_param_hw_handle_t param_hd;
    int16_t width[2] = { cfg.cfg0.res.width, cfg.cfg1.res.width };
    int16_t height[2] = { cfg.cfg0.res.height, cfg.cfg1.res.height };
    out_length[0] = (cfg.cfg0.res.width * cfg.cfg0.res.height + (cfg.cfg0.res.width * cfg.cfg0.res.height >> 1));
    out_length[1] = (cfg.cfg1.res.width * cfg.cfg1.res.height + (cfg.cfg1.res.width * cfg.cfg1.res.height >> 1));
    esp_h264_enc_roi_cfg_t roi_cfg = {
        .roi_mode = ESP_H264_ROI_MODE_DELTA_QP,
        .none_roi_delta_qp = 2,
    };
    esp_h264_enc_roi_reg_t roi_reg[8] = { 0 };
    esp_h264_enc_roi_reg_t roi_reg1[8] = { 0 };
    esp_h264_enc_cfg_t base_cfg;
    int index_c = 0;
    for (int16_t i = 0; i < 2; i++) {
        in_frame[i] = heap_caps_calloc(1, sizeof(esp_h264_enc_in_frame_t), MALLOC_CAP_INTERNAL);
        if (!in_frame[i]) {
            printf("mem allocation failed.line %d \n", __LINE__);
            goto _exit_dual_;
        }
        out_frame[i] = heap_caps_calloc(1, sizeof(esp_h264_enc_out_frame_t), MALLOC_CAP_INTERNAL);
        if (!out_frame[i]) {
            printf("mem allocation failed.line %d \n", __LINE__);
            goto _exit_dual_;
        }
        in_frame[i]->raw_data.len = out_length[i];
        in_frame[i]->raw_data.buffer = esp_h264_aligned_calloc(16, 1, in_frame[i]->raw_data.len, &in_frame[i]->raw_data.len, ESP_H264_MEM_INTERNAL);
        if (!in_frame[i]->raw_data.buffer) {
            printf("mem allocation failed.line %d \n", __LINE__);
            goto _exit_dual_;
        }
        out_frame[i]->raw_data.len = out_length[i];
        out_frame[i]->raw_data.buffer = esp_h264_aligned_calloc(16, 1, out_frame[i]->raw_data.len, &out_frame[i]->raw_data.len, ESP_H264_MEM_INTERNAL);
        if (!out_frame[i]->raw_data.buffer) {
            printf("mem allocation failed. line %d \n", __LINE__);
            goto _exit_dual_;
        }
        out_length[i] = out_frame[i]->raw_data.len;
    }

    ret = esp_h264_enc_dual_hw_new(&cfg, &enc);
    if (ret != ESP_H264_ERR_OK) {
        printf("new failed. line %d \n", __LINE__);
        goto _exit_dual_;
    }
    ret = esp_h264_enc_dual_open(enc);
    if (ret != ESP_H264_ERR_OK) {
        printf("open failed .line %d \n", __LINE__);
        goto _exit_dual_;
    }
    ret = esp_h264_enc_dual_hw_get_param_hd0(enc, &param_hd0);
    if (ret != ESP_H264_ERR_OK) {
        printf("esp_h264_enc_hw_get_param_hd error. line %d \n", __LINE__);
        goto _exit_dual_;
    }
    ret = esp_h264_enc_dual_hw_get_param_hd1(enc, &param_hd1);
    if (ret != ESP_H264_ERR_OK) {
        printf("esp_h264_enc_hw_get_param_hd error. line %d \n", __LINE__);
        goto _exit_dual_;
    }
    for (int8_t idx = ESP_H264_ROI_MODE_DISABLE; idx < ESP_H264_ROI_MODE_INVALID; idx++) {
        roi_cfg.roi_mode = idx;
        roi_cfg.none_roi_delta_qp = -26;
        while (1) {
            index_c ++;
            param_hd = index_c % 2 ? param_hd0 : param_hd1;
            memcpy(&base_cfg, index_c % 2 ? &cfg.cfg0 : &cfg.cfg1, sizeof(esp_h264_enc_cfg_t));
            roi_cfg.none_roi_delta_qp = (roi_cfg.none_roi_delta_qp + 1) % 51;
            ret = esp_h264_enc_hw_cfg_roi(param_hd, roi_cfg);
            if (ret != ESP_H264_ERR_OK) {
                printf("ROI configure error. line %d \n", __LINE__);
                goto _exit_dual_;
            }

            roi_reg[0].x = 0;
            roi_reg[0].y = 0;
            roi_reg[0].len_x = 0;
            roi_reg[0].len_y = 0;
            roi_reg[1].x = 0;
            roi_reg[1].y = 0;
            roi_reg[1].len_x = (base_cfg.res.width + 15) >> 4;
            roi_reg[1].len_y = ((base_cfg.res.height + 15) >> 4) >> 3;
            roi_reg[2].x = 0;
            roi_reg[2].y = 0;
            roi_reg[2].len_x = ((base_cfg.res.width + 15) >> 4) >> 3;
            roi_reg[2].len_y = (base_cfg.res.height + 15) >> 4;
            roi_reg[3].x = 0;
            roi_reg[3].y = ((base_cfg.res.height + 15) >> 4) * 7 >> 3;
            roi_reg[3].len_x = ((base_cfg.res.width + 15) >> 4);
            roi_reg[3].len_y = ((base_cfg.res.height + 15) >> 4) >> 3;
            roi_reg[4].x = ((base_cfg.res.width + 15) >> 4) * 7 >> 3;
            roi_reg[4].y = 0;
            roi_reg[4].len_x = ((base_cfg.res.width + 15) >> 4) >> 3;
            roi_reg[4].len_y = (base_cfg.res.height + 15) >> 4;
            roi_reg[5].x = ((base_cfg.res.width + 15) >> 4) >> 3;
            roi_reg[5].y = ((base_cfg.res.height + 15) >> 4) >> 3;
            roi_reg[5].len_x = ((base_cfg.res.width + 15) >> 4) >> 3;
            roi_reg[5].len_y = ((base_cfg.res.height + 15) >> 4) >> 3;
            roi_reg[6].x = ((base_cfg.res.width + 15) >> 4) * 2 >> 3;
            roi_reg[6].y = ((base_cfg.res.height + 15) >> 4) * 2 >> 3;
            roi_reg[6].len_x = ((base_cfg.res.width + 15) >> 4) >> 3;
            roi_reg[6].len_y = ((base_cfg.res.height + 15) >> 4) >> 3;
            roi_reg[7].x = ((base_cfg.res.width + 15) >> 4) * 3 >> 3;
            roi_reg[7].y = ((base_cfg.res.height + 15) >> 4) * 3 >> 3;
            roi_reg[7].len_x = ((base_cfg.res.width + 15) >> 4) * 2 >> 3;
            roi_reg[7].len_y = ((base_cfg.res.height + 15) >> 4) * 2 >> 3;
            if (idx > ESP_H264_ROI_MODE_DISABLE) {
                for (uint8_t i = 0; i < 8; i++) {
                    roi_reg[i].reg_idx = i;
                    if (roi_cfg.roi_mode == ESP_H264_ROI_MODE_FIX_QP) {
                        roi_reg[i].qp = ((i + index_c - 51) % 51) < 0 ? 0 : ((i + index_c - 51) % 51);
                    } else {
                        roi_reg[i].qp = (i + index_c - 51) % 51;
                    }
                    ret = esp_h264_enc_hw_set_roi_region(param_hd, roi_reg[i]);
                    if (ret != ESP_H264_ERR_OK) {
                        printf("ROI region error. line %d \n", __LINE__);
                        goto _exit_dual_;
                    }
                }
            }

            roi_reg[0].qp = 0;
            for (int16_t i = 0; i < 2; i++) {
                int ret_w = read_enc_cb_420(in_frame[i], width[i], height[i]);
                if (ret_w <= 0) {
                    goto _exit_dual_;
                }
            }

            ret = esp_h264_enc_dual_process(enc, in_frame, out_frame);
            if (ret != ESP_H264_ERR_OK) {
                printf("process failed. line %d \n", __LINE__);
                goto _exit_dual_;
            }
            for (int16_t i = 0; i < 2; i++) {
                write_enc_cb(out_frame[i]);
            }
            for (size_t i = 0; i < 8; i++) {
                roi_reg1[i].reg_idx = i;
                ret = esp_h264_enc_hw_get_roi_region(param_hd, &roi_reg1[i]);
                if (roi_cfg.roi_mode == ESP_H264_ROI_MODE_DISABLE) {
                    if (ret != ESP_H264_ERR_OK
                            || roi_reg1[i].x != 0
                            || roi_reg1[i].y != 0
                            || roi_reg1[i].len_x != 0
                            || roi_reg1[i].len_y != 0
                            || roi_reg1[i].qp != 0) {
                        goto _exit_dual_;
                    }
                } else {
                    if (ret != ESP_H264_ERR_OK
                            || roi_reg1[i].x != roi_reg[i].x
                            || roi_reg1[i].y != roi_reg[i].y
                            || roi_reg1[i].len_x != roi_reg[i].len_x
                            || roi_reg1[i].len_y != roi_reg[i].len_y
                            || roi_reg1[i].qp != roi_reg[i].qp) {
                        goto _exit_dual_;
                    }
                }
            }
        }
    }
_exit_dual_:
    ret |= esp_h264_enc_dual_close(enc);
    ret |= esp_h264_enc_dual_del(enc);
    for (int16_t i = 0; i < 2; i++) {
        if (in_frame[i]) {
            if (in_frame[i]->raw_data.buffer) {
                esp_h264_free(in_frame[i]->raw_data.buffer);
                esp_h264_free(in_frame[i]);
            }
        }
        if (out_frame[i]) {
            if (out_frame[i]->raw_data.buffer) {
                esp_h264_free(out_frame[i]->raw_data.buffer);
                esp_h264_free(out_frame[i]);
            }
        }
    }
    return ret;
}
/** MV component*/
esp_h264_err_t single_hw_enc_mv_pkt_test(esp_h264_enc_cfg_hw_t cfg)
{
    esp_h264_enc_in_frame_t in_frame = {0};
    esp_h264_enc_out_frame_t out_frame = {0};
    esp_h264_err_t ret = ESP_H264_ERR_FAIL;
    esp_h264_enc_handle_t enc = NULL;
    esp_h264_enc_param_hw_handle_t param_hd  = NULL;
    esp_h264_enc_mvm_pkt_t mv_pkt = {0};
    uint32_t length = 0;

    mv_pkt.len = ((cfg.res.width + 15) >> 4) * ((cfg.res.height + 15) >> 4);
    mv_pkt.len *= sizeof(*mv_pkt.data);
    mv_pkt.data = esp_h264_aligned_calloc(16, 1, mv_pkt.len, &mv_pkt.len, ESP_H264_MEM_INTERNAL);
    if (!mv_pkt.data) {
        printf("mem allocation failed.line %d \n", __LINE__);
        goto _mv_pkt_exit_;
    }

    in_frame.raw_data.len = (cfg.res.width * cfg.res.height + (cfg.res.width * cfg.res.height >> 1));
    in_frame.raw_data.buffer = esp_h264_aligned_calloc(16, 1, in_frame.raw_data.len, &in_frame.raw_data.len, ESP_H264_MEM_INTERNAL);
    if (!in_frame.raw_data.buffer) {
        printf("mem allocation failed.line %d \n", __LINE__);
        goto _mv_pkt_exit_;
    }
    out_frame.raw_data.len = (cfg.res.width * cfg.res.height + (cfg.res.width * cfg.res.height >> 1)) / 10;
    out_frame.raw_data.buffer = esp_h264_aligned_calloc(16, 1, out_frame.raw_data.len, &out_frame.raw_data.len, ESP_H264_MEM_INTERNAL);
    if (!out_frame.raw_data.buffer) {
        printf("mem allocation failed.line %d \n", __LINE__);
        goto _mv_pkt_exit_;
    }

    ret = esp_h264_enc_hw_new(&cfg, &enc);
    if (ret != ESP_H264_ERR_OK) {
        printf("new failed. line %d \n", __LINE__);
        goto _mv_pkt_exit_;
    }

    ret = esp_h264_enc_hw_get_param_hd(enc, &param_hd);
    if (ret != ESP_H264_ERR_OK) {
        printf("esp_h264_enc_hw_get_param_hd error. line %d \n", __LINE__);
        goto _mv_pkt_exit_;
    }
    ret = esp_h264_enc_open(enc);
    if (ret != ESP_H264_ERR_OK) {
        printf("open failed .line %d \n", __LINE__);
        goto _mv_pkt_exit_;
    }
    esp_h264_enc_mv_cfg_t mv_cfg = {
        .mv_mode = ESP_H264_MVM_MODE_DISABLE,
        .mv_fmt = ESP_H264_MVM_FMT_ALL,
    };
    for (int8_t mode = ESP_H264_MVM_MODE_DISABLE; mode < ESP_H264_MVM_MODE_INVALID; mode++) {
        for (int8_t fmt = ESP_H264_MVM_FMT_ALL; fmt < ESP_H264_MVM_FMT_INVALID; fmt++) {
            mv_cfg.mv_mode = mode;
            mv_cfg.mv_fmt = fmt;
            ret = esp_h264_enc_hw_cfg_mv(param_hd, mv_cfg);
            if (ret != ESP_H264_ERR_OK) {
                printf("open failed .line %d \n", __LINE__);
                goto _mv_pkt_exit_;
            }
            while (1) {
                int ret_w = read_enc_cb_420(&in_frame, cfg.res.width, cfg.res.height);
                if (ret_w <= 0) {
                    break;
                }
                ret = esp_h264_enc_hw_set_mv_pkt(param_hd, mv_pkt);
                if (ret != ESP_H264_ERR_OK) {
                    printf("esp_h264_enc_hw_set_mv_pkt failed. line %d \n", __LINE__);
                    goto _mv_pkt_exit_;
                }
                ret = esp_h264_enc_process(enc, &in_frame, &out_frame);
                if (ret != ESP_H264_ERR_OK) {
                    printf("process failed. line %d \n", __LINE__);
                    goto _mv_pkt_exit_;
                }
                write_enc_cb(&out_frame);
                ret = esp_h264_enc_hw_get_mv_data_len(param_hd, &length);
                if (ret != ESP_H264_ERR_OK) {
                    printf("esp_h264_enc_hw_get_mv_data_len failed. line %d \n", __LINE__);
                    goto _mv_pkt_exit_;
                }
                if (length > 0) {
                    write_mvm(&mv_pkt, length);
                }
            }
        }
    }
_mv_pkt_exit_:
    ret |= esp_h264_enc_close(enc);
    ret |= esp_h264_enc_del(enc);
    if (in_frame.raw_data.buffer) {
        esp_h264_free(in_frame.raw_data.buffer);
    }
    if (out_frame.raw_data.buffer) {
        esp_h264_free(out_frame.raw_data.buffer);
    }
    if (mv_pkt.data) {
        esp_h264_free(mv_pkt.data);
        mv_pkt.data = NULL;
    }
    return ret;
}

esp_h264_err_t dual_hw_enc_mv_pkt_test(esp_h264_enc_cfg_dual_hw_t cfg)
{
    esp_h264_enc_in_frame_t *in_frame[2] = {NULL, NULL};
    esp_h264_enc_out_frame_t *out_frame[2] = {NULL, NULL};
    esp_h264_err_t ret = ESP_H264_ERR_FAIL;
    esp_h264_enc_dual_handle_t enc = NULL;
    int16_t out_length[2];
    esp_h264_enc_param_hw_handle_t param_hd0 = NULL;
    esp_h264_enc_param_hw_handle_t param_hd1 = NULL;
    esp_h264_enc_param_hw_handle_t param_hd = NULL;
    int16_t width[2] = { cfg.cfg0.res.width, cfg.cfg1.res.width };
    int16_t height[2] = { cfg.cfg0.res.height, cfg.cfg1.res.height };
    out_length[0] = (cfg.cfg0.res.width * cfg.cfg0.res.height + (cfg.cfg0.res.width * cfg.cfg0.res.height >> 1));
    out_length[1] = (cfg.cfg1.res.width * cfg.cfg1.res.height + (cfg.cfg1.res.width * cfg.cfg1.res.height >> 1));
    esp_h264_enc_mv_cfg_t mv_cfg = {
        .mv_mode = ESP_H264_MVM_MODE_DISABLE,
        .mv_fmt = ESP_H264_MVM_FMT_ALL,
    };
    int index_c = 0;
    esp_h264_enc_mvm_pkt_t mv_pkt[2];
    mv_pkt[0].data = mv_pkt[1].data = NULL;
    uint32_t length = 0;
    esp_h264_enc_cfg_t base_cfg;
    esp_h264_enc_mvm_pkt_t mv_pkt_tmp;
    for (int16_t i = 0; i < 2; i++) {
        mv_pkt[i].len = ((width[i] + 15) >> 4) * ((height[i] + 15) >> 4);
        mv_pkt[i].len *= sizeof(*mv_pkt[i].data);
        mv_pkt[i].data = esp_h264_aligned_calloc(16, 1, mv_pkt[i].len, &mv_pkt[i].len, ESP_H264_MEM_INTERNAL);
        if (!mv_pkt[i].data) {
            printf("mem allocation failed.line %d \n", __LINE__);
            goto _exit_dual_;
        }
        in_frame[i] = heap_caps_calloc(1, sizeof(esp_h264_enc_in_frame_t), MALLOC_CAP_INTERNAL);
        if (!in_frame[i]) {
            printf("mem allocation failed.line %d \n", __LINE__);
            goto _exit_dual_;
        }
        out_frame[i] = heap_caps_calloc(1, sizeof(esp_h264_enc_out_frame_t), MALLOC_CAP_INTERNAL);
        if (!out_frame[i]) {
            printf("mem allocation failed.line %d \n", __LINE__);
            goto _exit_dual_;
        }
        in_frame[i]->raw_data.len = out_length[i];
        in_frame[i]->raw_data.buffer = esp_h264_aligned_calloc(16, 1, in_frame[i]->raw_data.len, &in_frame[i]->raw_data.len, ESP_H264_MEM_INTERNAL);
        if (!in_frame[i]->raw_data.buffer) {
            printf("mem allocation failed.line %d \n", __LINE__);
            goto _exit_dual_;
        }
        out_frame[i]->raw_data.len = out_length[i];
        out_frame[i]->raw_data.buffer = esp_h264_aligned_calloc(16, 1, out_frame[i]->raw_data.len, &out_frame[i]->raw_data.len, ESP_H264_MEM_INTERNAL);
        if (!out_frame[i]->raw_data.buffer) {
            printf("mem allocation failed. line %d \n", __LINE__);
            goto _exit_dual_;
        }
        out_length[i] = out_frame[i]->raw_data.len;
    }

    ret = esp_h264_enc_dual_hw_new(&cfg, &enc);
    if (ret != ESP_H264_ERR_OK) {
        printf("new failed. line %d \n", __LINE__);
        goto _exit_dual_;
    }
    ret = esp_h264_enc_dual_open(enc);
    if (ret != ESP_H264_ERR_OK) {
        printf("open failed .line %d \n", __LINE__);
        goto _exit_dual_;
    }
    ret = esp_h264_enc_dual_hw_get_param_hd0(enc, &param_hd0);
    if (ret != ESP_H264_ERR_OK) {
        printf("esp_h264_enc_hw_get_param_hd error. line %d \n", __LINE__);
        goto _exit_dual_;
    }
    ret = esp_h264_enc_dual_hw_get_param_hd1(enc, &param_hd1);
    if (ret != ESP_H264_ERR_OK) {
        printf("esp_h264_enc_hw_get_param_hd error. line %d \n", __LINE__);
        goto _exit_dual_;
    }
    for (int8_t mode = ESP_H264_MVM_MODE_DISABLE; mode < ESP_H264_MVM_MODE_INVALID; mode++) {
        for (int8_t fmt = ESP_H264_MVM_FMT_ALL; fmt < ESP_H264_MVM_FMT_INVALID; fmt++) {
            mv_cfg.mv_mode = mode;
            mv_cfg.mv_fmt = fmt;
            param_hd = index_c % 2 ? param_hd0 : param_hd1;
            memcpy(&mv_pkt_tmp, index_c % 2 ? &mv_pkt[0] : &mv_pkt[1], sizeof(esp_h264_enc_mvm_pkt_t));
            memcpy(&base_cfg, index_c % 2 ? &cfg.cfg0 : &cfg.cfg1, sizeof(esp_h264_enc_cfg_t));
            ret = esp_h264_enc_hw_cfg_mv(param_hd, mv_cfg);
            if (ret != ESP_H264_ERR_OK) {
                printf("open failed .line %d \n", __LINE__);
                goto _exit_dual_;
            }
            while (1) {
                index_c ++;
                for (int16_t i = 0; i < 2; i++) {
                    int ret_w = read_enc_cb_420(in_frame[i], width[i], height[i]);
                    if (ret_w <= 0) {
                        goto _exit_dual_;
                    }
                }
                ret = esp_h264_enc_hw_set_mv_pkt(param_hd, mv_pkt_tmp);
                if (ret != ESP_H264_ERR_OK) {
                    printf("esp_h264_enc_hw_set_mv_pkt failed. line %d \n", __LINE__);
                    goto _exit_dual_;
                }
                ret = esp_h264_enc_dual_process(enc, in_frame, out_frame);
                if (ret != ESP_H264_ERR_OK) {
                    printf("process failed. line %d \n", __LINE__);
                    goto _exit_dual_;
                }
                for (int16_t i = 0; i < 2; i++) {
                    write_enc_cb(out_frame[i]);
                }
                ret = esp_h264_enc_hw_get_mv_data_len(param_hd, &length);
                if (ret != ESP_H264_ERR_OK) {
                    printf("esp_h264_enc_hw_get_mv_data_len failed. line %d \n", __LINE__);
                    goto _exit_dual_;
                }

                if (length > 0) {
                    write_mvm(&mv_pkt_tmp, length);
                }
            }
        }
    }
_exit_dual_:
    ret |= esp_h264_enc_dual_close(enc);
    ret |= esp_h264_enc_dual_del(enc);
    for (int16_t i = 0; i < 2; i++) {
        if (in_frame[i]) {
            if (in_frame[i]->raw_data.buffer) {
                esp_h264_free(in_frame[i]->raw_data.buffer);
                esp_h264_free(in_frame[i]);
            }
        }
        if (out_frame[i]) {
            if (out_frame[i]->raw_data.buffer) {
                esp_h264_free(out_frame[i]->raw_data.buffer);
                esp_h264_free(out_frame[i]);
            }
        }
        if (mv_pkt[i].data) {
            esp_h264_free(mv_pkt[i].data);
            mv_pkt[i].data = NULL;
        }
    }
    return ret;
}
