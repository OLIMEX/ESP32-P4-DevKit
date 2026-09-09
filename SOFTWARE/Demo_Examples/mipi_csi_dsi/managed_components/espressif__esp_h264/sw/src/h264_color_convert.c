/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

void yuyv2iyuv(uint32_t height, uint32_t width, uint8_t *in, uint8_t *out)
{
    uint8_t *y = out;
    uint8_t *u = y + (width * height);
    uint8_t *v = u + (width * height >> 2);
    for (uint32_t i = 0; i < height; i += 2) {
        for (int j = 0; j < width; j += 2) {
            *(y++) = *(in++);
            *(u++) = *(in++);
            *(y++) = *(in++);
            *(v++) = *(in++);
        }
        for (uint32_t j = 0; j < width; j += 2) {
            *(y++) = *in;
            in += 2;
            *(y++) = *in;
            in += 2;
        }
    }
}
