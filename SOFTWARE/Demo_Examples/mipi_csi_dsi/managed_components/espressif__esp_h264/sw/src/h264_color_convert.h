/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*convert_color)(uint32_t height, uint32_t width, uint8_t *in, uint8_t *out);

#ifdef HAVE_ESP32S3

/**
 * @brief  Convert YUYV data to I420 data using ASM in esp32s3
 *
 * @param  height  Height of picture
 * @param  width   Width of picture
 * @param  in      YUYV data address
 * @param  out     I420 data address
 */

void yuyv2iyuv_esp32s3(uint32_t height, uint32_t width, uint8_t *in, uint8_t *out);

#endif

/**
 * @brief  Convert YUYV data to I420 data
 *
 * @param  height  Height of picture
 * @param  width   Width of picture
 * @param  in      YUYV data address
 * @param  out     I420 data address
 */
void yuyv2iyuv(uint32_t height, uint32_t width, uint8_t *in, uint8_t *out);

#ifdef __cplusplus
}
#endif
