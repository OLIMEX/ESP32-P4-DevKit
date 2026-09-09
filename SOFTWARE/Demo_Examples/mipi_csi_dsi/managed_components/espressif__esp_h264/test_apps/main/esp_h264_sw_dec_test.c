/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include "esp_h264_dec_param.h"
#include "esp_h264_sw_dec_test.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "h264_io.h"

esp_h264_err_t single_sw_dec_process(esp_h264_dec_cfg_sw_t cfg, uint8_t *inbuf, uint32_t inbuf_len, uint8_t *yuv)
{
    esp_h264_dec_in_frame_t in_frame;
    esp_h264_dec_out_frame_t out_frame;
    esp_h264_err_t ret = ESP_H264_ERR_FAIL;
    esp_h264_dec_handle_t dec = NULL;
    ret = esp_h264_dec_sw_new(&cfg, &dec);
    if (ret != ESP_H264_ERR_OK) {
        printf("new failed. line %d \n", __LINE__);
        goto _single_exit_;
    }

    ret = esp_h264_dec_open(dec);
    if (ret != ESP_H264_ERR_OK) {
        printf("open failed .line %d \n", __LINE__);
        goto _single_exit_;
    }
    int ret_w;
    ret_w = read_dec_cd(inbuf, inbuf_len, &in_frame);
    if (ret_w <= 0) {
        goto _single_exit_;
    }
    while (1) {
        if (in_frame.raw_data.len <= 0) {
            break;
        }
        ret = esp_h264_dec_process(dec, &in_frame, &out_frame);

        in_frame.raw_data.buffer += in_frame.consume;
        in_frame.raw_data.len -= in_frame.consume;
        if (ret == ESP_H264_ERR_OK) {
            write_dec_cd(&out_frame, NULL);
        } else {
            printf("process failed. ret %d line %d \n", ret, __LINE__);
            goto _single_exit_;
        }
    }
_single_exit_:
    ret |= esp_h264_dec_close(dec);
    ret |= esp_h264_dec_del(dec);
    return ret;
}
