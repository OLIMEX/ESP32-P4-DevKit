/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <string.h>
#include "esp_h264_types.h"

int read_enc_cb_420(esp_h264_enc_in_frame_t *frame, int16_t width, int16_t height);

int write_enc_cb(esp_h264_enc_out_frame_t *frame);

int read_enc_cb_i420(esp_h264_enc_in_frame_t *frame, int16_t width, int16_t height);

int read_dec_cd(uint8_t *inbuf, uint32_t inbuf_len, esp_h264_dec_in_frame_t *frame);

int write_dec_cd(esp_h264_dec_out_frame_t *frame, uint8_t *yuv);
