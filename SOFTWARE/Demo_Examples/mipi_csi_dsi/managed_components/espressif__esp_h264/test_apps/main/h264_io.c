/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "h264_io.h"

const uint8_t yuv_table[38][3] = {
    { 0xd6, 0x71, 0xb8 },
    { 0x7e, 0x6a, 0xa0 },
    { 0x41, 0x81, 0x78 },
    { 0xde, 0x39, 0x92 },
    { 0x66, 0xa1, 0x71 },
    { 0x61, 0x71, 0xc5 },
    { 0x99, 0x73, 0x93 },
    { 0x3b, 0x74, 0x98 },
    { 0x57, 0x89, 0x7c },
    { 0x88, 0x57, 0x9b },
    { 0x48, 0x93, 0x7b },
    { 0x51, 0x5e, 0x61 },
    { 0x89, 0x9d, 0x7e },
    { 0x33, 0x70, 0xb4 },
    { 0xde, 0x39, 0x92 },
    { 0x66, 0xa1, 0x71 },
    { 0x61, 0x71, 0xc5 },
    { 0x61, 0x71, 0xc5 },
    { 0x61, 0x71, 0xc5 },
    { 0x61, 0x71, 0xc5 },
    { 0x61, 0x71, 0xc5 },
    { 0x61, 0x71, 0xc5 },
    { 0x61, 0x71, 0xc5 },
    { 0x61, 0x71, 0xc5 },
    { 0x61, 0x71, 0xc5 },
    { 0x61, 0x71, 0xc5 },
    { 0x61, 0x71, 0xc5 },
    { 0x61, 0x71, 0xc5 },
    { 0x61, 0x71, 0xc5 },
    { 0x61, 0x71, 0xc5 },
    { 0x61, 0x71, 0xc5 },
    { 0x61, 0x71, 0xc5 },
    { 0x61, 0x71, 0xc5 },
    { 0x99, 0x73, 0x93 },
    { 0x66, 0xa1, 0x71 },
    { 0x61, 0x71, 0xc5 },
    { 0x3b, 0x74, 0x98 },
    { 0x57, 0x89, 0x7c },
};

static int index_c = 0;
#define COLOR_NUM (sizeof(yuv_table) / sizeof(yuv_table[0]))

int read_enc_cb_420(esp_h264_enc_in_frame_t *frame, int16_t width, int16_t height)
{
    index_c++;
    if (index_c > COLOR_NUM) {
        index_c = 0;
        return -1;
    }
    int cor_idx = index_c % COLOR_NUM;
    uint8_t y = yuv_table[cor_idx][0];
    uint8_t u = yuv_table[cor_idx][1];
    uint8_t v = yuv_table[cor_idx][2];
    uint8_t *yuv = frame->raw_data.buffer;
    for (int j = 0; j < height; j += 2) {
        for (int i = 0; i < width; i += 16) {
            *yuv++ = u; // U0
            *yuv++ = y; // Y0
            *yuv++ = y; // Y1
            *yuv++ = u; // U1
            *yuv++ = y; // Y2
            *yuv++ = y; // Y3
            *yuv++ = u; // U2
            *yuv++ = y; // Y4

            *yuv++ = y; // Y5
            *yuv++ = u; // U3
            *yuv++ = y; // Y6
            *yuv++ = y; // Y7
            *yuv++ = u; // U4
            *yuv++ = y; // Y8
            *yuv++ = y; // Y9
            *yuv++ = u; // U5

            *yuv++ = y; // Y10
            *yuv++ = y; // Y11
            *yuv++ = u; // U6
            *yuv++ = y; // Y12
            *yuv++ = y; // Y13
            *yuv++ = u; // U7
            *yuv++ = y; // Y15
            *yuv++ = y; // Y14
        }
        for (int i = 0; i < width; i += 16) {
            *yuv++ = v; // V0
            *yuv++ = y; // Y0
            *yuv++ = y; // Y1
            *yuv++ = v; // V1
            *yuv++ = y; // Y2
            *yuv++ = y; // Y3
            *yuv++ = v; // V2
            *yuv++ = y; // Y4

            *yuv++ = y; // Y5
            *yuv++ = v; // V3
            *yuv++ = y; // Y6
            *yuv++ = y; // Y7
            *yuv++ = v; // V4
            *yuv++ = y; // Y8
            *yuv++ = y; // Y9
            *yuv++ = v; // V5

            *yuv++ = y; // Y10
            *yuv++ = y; // Y11
            *yuv++ = v; // V6
            *yuv++ = y; // Y12
            *yuv++ = y; // Y13
            *yuv++ = v; // V7
            *yuv++ = y; // Y15
            *yuv++ = y; // Y14
        }
    }
    return 1;
}

int write_enc_cb(esp_h264_enc_out_frame_t *frame)
{
    return 1;
}

int read_enc_cb_i420(esp_h264_enc_in_frame_t *frame, int16_t width, int16_t height)
{
    index_c++;
    if (index_c > COLOR_NUM) {
        index_c = 0;
        return -1;
    }
    int cor_idx = index_c % COLOR_NUM;
    uint8_t y = yuv_table[cor_idx][0];
    uint8_t u = yuv_table[cor_idx][1];
    uint8_t v = yuv_table[cor_idx][2];
    uint8_t *yuv = frame->raw_data.buffer;
    for (int j = 0; j < height * width; j++) {
        *yuv++ = y; // Y
    }
    for (int j = 0; j < height *width >> 2; j++) {
        *yuv++ = u; // u
    }
    for (int j = 0; j < height *width >> 2; j++) {
        *yuv++ = v; // v
    }
    return 1;
}

int read_dec_cd(uint8_t *inbuf, uint32_t inbuf_len, esp_h264_dec_in_frame_t *frame)
{
    frame->raw_data.buffer = inbuf;
    frame->raw_data.len = inbuf_len;
    return 1;
}

int write_dec_cd(esp_h264_dec_out_frame_t *frame, uint8_t *yuv)
{
    return frame->out_size;
}
