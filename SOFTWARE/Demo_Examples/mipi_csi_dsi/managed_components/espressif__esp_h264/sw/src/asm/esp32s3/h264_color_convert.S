/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
 
#include <xtensa/coreasm.h>
#include <xtensa/corebits.h>
#include <xtensa/config/system.h>

#define height a2
#define width a3
#define in a4
#define out a5
#define out_u a6
#define out_v a7
#define tmp a8

    .section .iram1,"ax"
    .global     yuyv2iyuv_esp32s3
    .type       yuyv2iyuv_esp32s3,@function
    .align      4
yuyv2iyuv_esp32s3:
    entry a1, 32
    mull tmp, height, width
    add out_u, out, tmp
    srli tmp, tmp, 2
    add out_v, out_u, tmp
loop_end0:
    srli tmp, width, 5
    loopnez tmp, loop_end1
        ee.vld.128.ip q0, in, 16
        ee.vld.128.ip q1, in, 16
        ee.vunzip.8 q0, q1
        ee.vst.128.ip q0, out, 16
        ee.vld.128.ip q2, in, 16
        ee.vld.128.ip q3, in, 16
        ee.vunzip.8 q2, q3
        ee.vst.128.ip q2, out, 16
        ee.vunzip.8 q1, q3
        ee.vst.128.ip q1, out_u, 16
        ee.vst.128.ip q3, out_v, 16
    loop_end1: 
        srli tmp, width, 5
    loopnez tmp, loop_end2
        ee.vld.128.ip q0, in, 16
        ee.vld.128.ip q1, in, 16
        ee.vunzip.8 q0, q1
        ee.vst.128.ip q0, out, 16
        ee.vld.128.ip q2, in, 16
        ee.vld.128.ip q3, in, 16
        ee.vunzip.8 q2, q3
        ee.vst.128.ip q2, out, 16
    loop_end2: 
    addi height, height, -2
    bgei height, 1, loop_end0
retw
