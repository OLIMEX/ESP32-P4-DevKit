/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdio.h>
#include <stdint.h>
#include <sdkconfig.h>
#include "sc202cs_regs.h"
#include "sc202cs_types.h"

#ifdef __cplusplus
extern "C" {
#endif

static const sc202cs_reginfo_t init_reglist_MIPI_1lane_raw10_1600x1200_30fps[] = {
    {0x0103, 0x01},
    {SC202CS_REG_SLEEP_MODE, 0x00},
    {0x36e9, 0x80},
    {0x36e9, 0x24},
    {0x301f, 0x01},
    {0x3301, 0xff},
    {0x3304, 0x68},
    {0x3306, 0x40},
    {0x3308, 0x08},
    {0x3309, 0xa8},
    {0x330b, 0xb0},
    {0x330c, 0x18},
    {0x330d, 0xff},
    {0x330e, 0x20},
    {0x331e, 0x59},
    {0x331f, 0x99},
    {0x3333, 0x10},
    {0x335e, 0x06},
    {0x335f, 0x08},
    {0x3364, 0x1f},
    {0x337c, 0x02},
    {0x337d, 0x0a},
    {0x338f, 0xa0},
    {0x3390, 0x01},
    {0x3391, 0x03},
    {0x3392, 0x1f},
    {0x3393, 0xff},
    {0x3394, 0xff},
    {0x3395, 0xff},
    {0x33a2, 0x04},
    {0x33ad, 0x0c},
    {0x33b1, 0x20},
    {0x33b3, 0x38},
    {0x33f9, 0x40},
    {0x33fb, 0x48},
    {0x33fc, 0x0f},
    {0x33fd, 0x1f},
    {0x349f, 0x03},
    {0x34a6, 0x03},
    {0x34a7, 0x1f},
    {0x34a8, 0x38},
    {0x34a9, 0x30},
    {0x34ab, 0xb0},
    {0x34ad, 0xb0},
    {0x34f8, 0x1f},
    {0x34f9, 0x20},
    {0x3630, 0xa0},
    {0x3631, 0x92},
    {0x3632, 0x64},
    {0x3633, 0x43},
    {0x3637, 0x49},
    {0x363a, 0x85},
    {0x363c, 0x0f},
    {0x3650, 0x31},
    {0x3670, 0x0d},
    {0x3674, 0xc0},
    {0x3675, 0xa0},
    {0x3676, 0xa0},
    {0x3677, 0x92},
    {0x3678, 0x96},
    {0x3679, 0x9a},
    {0x367c, 0x03},
    {0x367d, 0x0f},
    {0x367e, 0x01},
    {0x367f, 0x0f},
    {0x3698, 0x83},
    {0x3699, 0x86},
    {0x369a, 0x8c},
    {0x369b, 0x94},
    {0x36a2, 0x01},
    {0x36a3, 0x03},
    {0x36a4, 0x07},
    {0x36ae, 0x0f},
    {0x36af, 0x1f},
    {0x36bd, 0x22},
    {0x36be, 0x22},
    {0x36bf, 0x22},
    {0x36d0, 0x01},
    {0x370f, 0x02},
    {0x3721, 0x6c},
    {0x3722, 0x8d},
    {0x3725, 0xc5},
    {0x3727, 0x14},
    {0x3728, 0x04},
    {0x37b7, 0x04},
    {0x37b8, 0x04},
    {0x37b9, 0x06},
    {0x37bd, 0x07},
    {0x37be, 0x0f},
    {0x3901, 0x02},
    {0x3903, 0x40},
    {0x3905, 0x8d},
    {0x3907, 0x00},
    {0x3908, 0x41},
    {0x391f, 0x41},
    {0x3933, 0x80},
    {0x3934, 0x02},
    {0x3937, 0x6f},
    {0x393a, 0x01},
    {0x393d, 0x01},
    {0x393e, 0xc0},
    {0x39dd, 0x41},
    {0x3e00, 0x00},
    {0x3e01, 0x4d},
    {0x3e02, 0xc0},
    {0x3e09, 0x00},
    {0x4509, 0x28},
    {0x450d, 0x61},
    {SC202CS_REG_END, 0x00},
};

static const sc202cs_reginfo_t init_reglist_MIPI_1lane_raw10_1600x900_30fps[] = {
    {0x0103, 0x01},
    {SC202CS_REG_SLEEP_MODE, 0x00},
    {0x36e9, 0x80},
    {0x36e9, 0x24},
    {0x301f, 0x01},
    {0x3301, 0xff},
    {0x3304, 0x68},
    {0x3306, 0x40},
    {0x3308, 0x08},
    {0x3309, 0xa8},
    {0x330b, 0xb0},
    {0x330c, 0x18},
    {0x330d, 0xff},
    {0x330e, 0x20},
    {0x331e, 0x59},
    {0x331f, 0x99},
    {0x3333, 0x10},
    {0x335e, 0x06},
    {0x335f, 0x08},
    {0x3364, 0x1f},
    {0x337c, 0x02},
    {0x337d, 0x0a},
    {0x338f, 0xa0},
    {0x3390, 0x01},
    {0x3391, 0x03},
    {0x3392, 0x1f},
    {0x3393, 0xff},
    {0x3394, 0xff},
    {0x3395, 0xff},
    {0x33a2, 0x04},
    {0x33ad, 0x0c},
    {0x33b1, 0x20},
    {0x33b3, 0x38},
    {0x33f9, 0x40},
    {0x33fb, 0x48},
    {0x33fc, 0x0f},
    {0x33fd, 0x1f},
    {0x349f, 0x03},
    {0x34a6, 0x03},
    {0x34a7, 0x1f},
    {0x34a8, 0x38},
    {0x34a9, 0x30},
    {0x34ab, 0xb0},
    {0x34ad, 0xb0},
    {0x34f8, 0x1f},
    {0x34f9, 0x20},
    {0x3630, 0xa0},
    {0x3631, 0x92},
    {0x3632, 0x64},
    {0x3633, 0x43},
    {0x3637, 0x49},
    {0x363a, 0x85},
    {0x363c, 0x0f},
    {0x3650, 0x31},
    {0x3670, 0x0d},
    {0x3674, 0xc0},
    {0x3675, 0xa0},
    {0x3676, 0xa0},
    {0x3677, 0x92},
    {0x3678, 0x96},
    {0x3679, 0x9a},
    {0x367c, 0x03},
    {0x367d, 0x0f},
    {0x367e, 0x01},
    {0x367f, 0x0f},
    {0x3698, 0x83},
    {0x3699, 0x86},
    {0x369a, 0x8c},
    {0x369b, 0x94},
    {0x36a2, 0x01},
    {0x36a3, 0x03},
    {0x36a4, 0x07},
    {0x36ae, 0x0f},
    {0x36af, 0x1f},
    {0x36bd, 0x22},
    {0x36be, 0x22},
    {0x36bf, 0x22},
    {0x36d0, 0x01},
    {0x370f, 0x02},
    {0x3721, 0x6c},
    {0x3722, 0x8d},
    {0x3725, 0xc5},
    {0x3727, 0x14},
    {0x3728, 0x04},
    {0x37b7, 0x04},
    {0x37b8, 0x04},
    {0x37b9, 0x06},
    {0x37bd, 0x07},
    {0x37be, 0x0f},
    {0x3901, 0x02},
    {0x3903, 0x40},
    {0x3905, 0x8d},
    {0x3907, 0x00},
    {0x3908, 0x41},
    {0x391f, 0x41},
    {0x3933, 0x80},
    {0x3934, 0x02},
    {0x3937, 0x6f},
    {0x393a, 0x01},
    {0x393d, 0x01},
    {0x393e, 0xc0},
    {0x39dd, 0x41},
    {0x3e00, 0x00},
    {0x3e01, 0x4d},
    {0x3e02, 0xc0},
    {0x3e09, 0x00},
    {0x4509, 0x28},
    {0x450d, 0x61},
    {0x3200, 0x00},
    {0x3201, 0x00},
    {0x3202, 0x00},
    {0x3203, 0x96},
    {0x3204, 0x06},
    {0x3205, 0x47},
    {0x3206, 0x04},
    {0x3207, 0x21},
    {0x3208, 0x06},
    {0x3209, 0x40},
    {0x320a, 0x03},
    {0x320b, 0x84},
    {0x3210, 0x00},
    {0x3211, 0x04},
    {0x3212, 0x00},
    {0x3213, 0x04},
    {0x320E, 0x04}, //30FPS
    {0x320F, 0xE2},
    {SC202CS_REG_END, 0x00},
};

// cleaned_0x18_FT_SC2356_24Minput_576Mbps_1lane_8bit_1280x720_30fps
static const sc202cs_reginfo_t init_reglist_MIPI_1lane_raw8_1280x720_30fps[] = {
    {0x0103, 0x01},
    {SC202CS_REG_SLEEP_MODE, 0x00},
    {0x36e9, 0x80},
    {0x36ea, 0x06},
    {0x36eb, 0x0a},
    {0x36ec, 0x01},
    {0x36ed, 0x18},
    {0x36e9, 0x24},
    {0x301f, 0x18},
    {0x3031, 0x08},
    {0x3037, 0x00},
    {0x3200, 0x00},
    {0x3201, 0xa0},
    {0x3202, 0x00},
    {0x3203, 0xf0},
    {0x3204, 0x05},
    {0x3205, 0xa7},
    {0x3206, 0x03},
    {0x3207, 0xc7},
    {0x3208, 0x05},
    {0x3209, 0x00},
    {0x320a, 0x02},
    {0x320b, 0xd0},
    {0x3210, 0x00},
    {0x3211, 0x04},
    {0x3212, 0x00},
    {0x3213, 0x04},
    {0x3301, 0xff},
    {0x3304, 0x68},
    {0x3306, 0x40},
    {0x3308, 0x08},
    {0x3309, 0xa8},
    {0x330b, 0xd0},
    {0x330c, 0x18},
    {0x330d, 0xff},
    {0x330e, 0x20},
    {0x331e, 0x59},
    {0x331f, 0x99},
    {0x3333, 0x10},
    {0x335e, 0x06},
    {0x335f, 0x08},
    {0x3364, 0x1f},
    {0x337c, 0x02},
    {0x337d, 0x0a},
    {0x338f, 0xa0},
    {0x3390, 0x01},
    {0x3391, 0x03},
    {0x3392, 0x1f},
    {0x3393, 0xff},
    {0x3394, 0xff},
    {0x3395, 0xff},
    {0x33a2, 0x04},
    {0x33ad, 0x0c},
    {0x33b1, 0x20},
    {0x33b3, 0x38},
    {0x33f9, 0x40},
    {0x33fb, 0x48},
    {0x33fc, 0x0f},
    {0x33fd, 0x1f},
    {0x349f, 0x03},
    {0x34a6, 0x03},
    {0x34a7, 0x1f},
    {0x34a8, 0x38},
    {0x34a9, 0x30},
    {0x34ab, 0xd0},
    {0x34ad, 0xd8},
    {0x34f8, 0x1f},
    {0x34f9, 0x20},
    {0x3630, 0xa0},
    {0x3631, 0x92},
    {0x3632, 0x64},
    {0x3633, 0x43},
    {0x3637, 0x49},
    {0x363a, 0x85},
    {0x363c, 0x0f},
    {0x3650, 0x31},
    {0x3670, 0x0d},
    {0x3674, 0xc0},
    {0x3675, 0xa0},
    {0x3676, 0xa0},
    {0x3677, 0x92},
    {0x3678, 0x96},
    {0x3679, 0x9a},
    {0x367c, 0x03},
    {0x367d, 0x0f},
    {0x367e, 0x01},
    {0x367f, 0x0f},
    {0x3698, 0x83},
    {0x3699, 0x86},
    {0x369a, 0x8c},
    {0x369b, 0x94},
    {0x36a2, 0x01},
    {0x36a3, 0x03},
    {0x36a4, 0x07},
    {0x36ae, 0x0f},
    {0x36af, 0x1f},
    {0x36bd, 0x22},
    {0x36be, 0x22},
    {0x36bf, 0x22},
    {0x36d0, 0x01},
    {0x370f, 0x02},
    {0x3721, 0x6c},
    {0x3722, 0x8d},
    {0x3725, 0xc5},
    {0x3727, 0x14},
    {0x3728, 0x04},
    {0x37b7, 0x04},
    {0x37b8, 0x04},
    {0x37b9, 0x06},
    {0x37bd, 0x07},
    {0x37be, 0x0f},
    {0x3901, 0x02},
    {0x3903, 0x40},
    {0x3905, 0x8d},
    {0x3907, 0x00},
    {0x3908, 0x41},
    {0x391f, 0x41},
    {0x3933, 0x80},
    {0x3934, 0x02},
    {0x3937, 0x6f},
    {0x393a, 0x01},
    {0x393d, 0x01},
    {0x393e, 0xc0},
    {0x39dd, 0x41},
    {0x3e00, 0x00},
    {0x3e01, 0x4d},
    {0x3e02, 0xc0},
    {0x3e09, 0x00},
    {0x4509, 0x28},
    {0x450d, 0x61},
    {SC202CS_REG_END, 0x00},
};

// cleaned_0x18_FT_SC2356_24Minput_576Mbps_1lane_8bit_1600x1200_30fps
static const sc202cs_reginfo_t init_reglist_MIPI_1lane_raw8_1600x1200_30fps[] = {
    {0x0103, 0x01},
    {SC202CS_REG_SLEEP_MODE, 0x00},
    {0x36e9, 0x80},
    {0x36ea, 0x06},
    {0x36eb, 0x0a},
    {0x36ec, 0x01},
    {0x36ed, 0x18},
    {0x36e9, 0x24},
    {0x301f, 0x18},
    {0x3031, 0x08},
    {0x3037, 0x00},
    {0x3301, 0xff},
    {0x3304, 0x68},
    {0x3306, 0x40},
    {0x3308, 0x08},
    {0x3309, 0xa8},
    {0x330b, 0xd0},
    {0x330c, 0x18},
    {0x330d, 0xff},
    {0x330e, 0x20},
    {0x331e, 0x59},
    {0x331f, 0x99},
    {0x3333, 0x10},
    {0x335e, 0x06},
    {0x335f, 0x08},
    {0x3364, 0x1f},
    {0x337c, 0x02},
    {0x337d, 0x0a},
    {0x338f, 0xa0},
    {0x3390, 0x01},
    {0x3391, 0x03},
    {0x3392, 0x1f},
    {0x3393, 0xff},
    {0x3394, 0xff},
    {0x3395, 0xff},
    {0x33a2, 0x04},
    {0x33ad, 0x0c},
    {0x33b1, 0x20},
    {0x33b3, 0x38},
    {0x33f9, 0x40},
    {0x33fb, 0x48},
    {0x33fc, 0x0f},
    {0x33fd, 0x1f},
    {0x349f, 0x03},
    {0x34a6, 0x03},
    {0x34a7, 0x1f},
    {0x34a8, 0x38},
    {0x34a9, 0x30},
    {0x34ab, 0xd0},
    {0x34ad, 0xd8},
    {0x34f8, 0x1f},
    {0x34f9, 0x20},
    {0x3630, 0xa0},
    {0x3631, 0x92},
    {0x3632, 0x64},
    {0x3633, 0x43},
    {0x3637, 0x49},
    {0x363a, 0x85},
    {0x363c, 0x0f},
    {0x3650, 0x31},
    {0x3670, 0x0d},
    {0x3674, 0xc0},
    {0x3675, 0xa0},
    {0x3676, 0xa0},
    {0x3677, 0x92},
    {0x3678, 0x96},
    {0x3679, 0x9a},
    {0x367c, 0x03},
    {0x367d, 0x0f},
    {0x367e, 0x01},
    {0x367f, 0x0f},
    {0x3698, 0x83},
    {0x3699, 0x86},
    {0x369a, 0x8c},
    {0x369b, 0x94},
    {0x36a2, 0x01},
    {0x36a3, 0x03},
    {0x36a4, 0x07},
    {0x36ae, 0x0f},
    {0x36af, 0x1f},
    {0x36bd, 0x22},
    {0x36be, 0x22},
    {0x36bf, 0x22},
    {0x36d0, 0x01},
    {0x370f, 0x02},
    {0x3721, 0x6c},
    {0x3722, 0x8d},
    {0x3725, 0xc5},
    {0x3727, 0x14},
    {0x3728, 0x04},
    {0x37b7, 0x04},
    {0x37b8, 0x04},
    {0x37b9, 0x06},
    {0x37bd, 0x07},
    {0x37be, 0x0f},
    {0x3901, 0x02},
    {0x3903, 0x40},
    {0x3905, 0x8d},
    {0x3907, 0x00},
    {0x3908, 0x41},
    {0x391f, 0x41},
    {0x3933, 0x80},
    {0x3934, 0x02},
    {0x3937, 0x6f},
    {0x393a, 0x01},
    {0x393d, 0x01},
    {0x393e, 0xc0},
    {0x39dd, 0x41},
    {0x3e00, 0x00},
    {0x3e01, 0x4d},
    {0x3e02, 0xc0},
    {0x3e09, 0x00},
    {0x4509, 0x28},
    {0x450d, 0x61},
    {SC202CS_REG_END, 0x00},
};

#ifdef __cplusplus
}
#endif