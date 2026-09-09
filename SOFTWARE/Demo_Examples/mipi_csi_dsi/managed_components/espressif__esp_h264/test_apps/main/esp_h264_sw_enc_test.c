/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_h264_sw_enc_test.h"
#include "esp_h264_alloc.h"
#include "h264_io.h"

/** GOP FPS RC */
esp_h264_err_t single_sw_enc_thread_test(esp_h264_enc_cfg_sw_t cfg)
{
    esp_h264_enc_in_frame_t in_frame;
    esp_h264_enc_out_frame_t out_frame;
    esp_h264_err_t ret = ESP_H264_ERR_FAIL;
    esp_h264_enc_handle_t enc = NULL;
    esp_h264_resolution_t res;
    esp_h264_enc_rc_t rc;
    uint8_t gop;
    uint8_t fps;
    esp_h264_enc_param_handle_t param_hd;
    int index_c = 0;
    in_frame.raw_data.len = (cfg.res.width * cfg.res.height + (cfg.res.width * cfg.res.height >> 1));
    in_frame.raw_data.buffer = esp_h264_aligned_calloc(16, 1, in_frame.raw_data.len, &in_frame.raw_data.len, MALLOC_CAP_INTERNAL);
    if (!in_frame.raw_data.buffer) {
        printf("mem allocation failed.line %d \n", __LINE__);
        goto _exit_;
    }
    out_frame.raw_data.len = (cfg.res.width * cfg.res.height + (cfg.res.width * cfg.res.height >> 1)) / 10;
    out_frame.raw_data.buffer = esp_h264_aligned_calloc(16, 1, out_frame.raw_data.len, &out_frame.raw_data.len, MALLOC_CAP_INTERNAL);
    if (!out_frame.raw_data.buffer) {
        printf("mem allocation failed.line %d \n", __LINE__);
        goto _exit_;
    }

    ret = esp_h264_enc_sw_new(&cfg, &enc);
    if (ret != ESP_H264_ERR_OK) {
        printf("new failed. line %d \n", __LINE__);
        goto _exit_;
    }

    ret = esp_h264_enc_sw_get_param_hd(enc, &param_hd);
    if (ret != ESP_H264_ERR_OK) {
        printf("esp_h264_enc_sw_get_param_hd error. line %d \n", __LINE__);
        goto _exit_;
    }

    ret = esp_h264_enc_get_resolution(param_hd, &res);
    if ((ret != ESP_H264_ERR_OK)
            || (res.width != cfg.res.width)
            || (res.height != cfg.res.height)) {
        printf("esp_h264_enc_get_resolution failed .line %d \n", __LINE__);
        goto _exit_;
    }

    ret = esp_h264_enc_get_fps(param_hd, &fps);
    if ((ret != ESP_H264_ERR_OK)
            || (fps != cfg.fps)) {
        printf("esp_h264_enc_get_fps failed .line %d \n", __LINE__);
        goto _exit_;
    }

    ret = esp_h264_enc_get_gop(param_hd, &gop);
    if ((ret != ESP_H264_ERR_OK)
            || (gop != cfg.gop)) {
        printf("esp_h264_enc_get_gop failed .line %d %d \n", gop, __LINE__);
        goto _exit_;
    }

    ret = esp_h264_enc_get_bitrate(param_hd, &rc.bitrate);
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
        int ret_w = read_enc_cb_i420(&in_frame, cfg.res.width, cfg.res.height);
        if (ret_w <= 0) {
            break;
        }
        ret = esp_h264_enc_get_resolution(param_hd, &res);
        if ((ret != ESP_H264_ERR_OK)
                || (res.width != cfg.res.width)
                || (res.height != cfg.res.height)) {
            printf("esp_h264_enc_get_resolution failed .line %d \n", __LINE__);
            goto _exit_;
        }

        ret = esp_h264_enc_get_fps(param_hd, &fps);
        if ((ret != ESP_H264_ERR_OK)
                || (fps != cfg.fps)) {
            printf("esp_h264_enc_get_fps failed .line %d \n", __LINE__);
            goto _exit_;
        }

        ret = esp_h264_enc_get_gop(param_hd, &gop);
        if ((ret != ESP_H264_ERR_OK)
                || (gop != cfg.gop)) {
            printf("esp_h264_enc_get_gop failed. GOP %d line %d \n", gop, __LINE__);
            goto _exit_;
        }

        ret = esp_h264_enc_get_bitrate(param_hd, &rc.bitrate);
        if (ret != ESP_H264_ERR_OK
                || (rc.bitrate != cfg.rc.bitrate)) {
            printf("esp_h264_enc_get_bitrate failed .line %d \n", __LINE__);
            printf("RC %d %d \n", (int)rc.bitrate, (int)cfg.rc.bitrate );
            goto _exit_;
        }

        cfg.fps = index_c + 4;
        ret = esp_h264_enc_set_fps(param_hd, cfg.fps);
        if (ret != ESP_H264_ERR_OK) {
            printf("esp_h264_enc_set_fps failed .line %d \n", __LINE__);
            goto _exit_;
        }
        cfg.gop = index_c + 3;
        ret = esp_h264_enc_set_gop(param_hd, cfg.gop);
        if (ret != ESP_H264_ERR_OK) {
            printf("esp_h264_enc_set_gop failed .line %d \n", __LINE__);
            goto _exit_;
        }

        cfg.rc.bitrate = cfg.res.width * cfg.res.height * cfg.fps / 20;
        ret = esp_h264_enc_set_bitrate(param_hd, cfg.rc.bitrate);
        if (ret != ESP_H264_ERR_OK) {
            printf("esp_h264_enc_set_bitrate failed .line %d \n", __LINE__);
            goto _exit_;
        }
        ret = esp_h264_enc_process(enc, &in_frame, &out_frame);
        if (ret != ESP_H264_ERR_OK) {
            printf("process failed. line %d \n", __LINE__);
            goto _exit_;
        }
        if (write_enc_cb(&out_frame) < 0) {
            ret = ESP_H264_ERR_FAIL;
            printf("data error. line %d \n", __LINE__);
            goto _exit_;
        };
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
