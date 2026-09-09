/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_H264_VERSION "1.0.3"

/**
 *  Features:
 *     - H264 hardware encoder
 *  *         - Support baseline profile (max frame size is 36864 macro-block)
 *            - Support a variety of widths in range[80, 1088] and heights in range [80, 2048]
 *            - Support quality first rate control
 *            - Support YUV420 raw data
 *            - Support dynamic changes about bit rate, frame rate, GOP , QP .ect.
 *            - Support single and dual stream encoder
 *            - Support de-blocking filter, ROI, MV function
 *            - Support SPS and PPS encoding
 *     software encoder
 *            - Support baseline profile (max frame size is 36864 macro-block)
 *            - Support a variety of widths and heights greater than 16
 *            - Support quality first rate control
 *            - Support YUYV and IYUV raw data
 *            - Support dynamic changes about bit rate, frame rate
 *            - Support more than one slice per frame
 *            - Support SPS and PPS encoding
 *     software decoder
 *            - Support baseline profile (max frame size is 36864 macro-block)
 *            - Support a variety of widths and heights
 *            - Support long term reference (LTR) frames
 *            - Support memory management control operation (MMCO)
 *            - Support reference picture list modification
 *            - Support multiple reference frames when specified in sequence parameter set (SPS)
 *            - Support IYUV output
 *  Release Notes:
 *     v1.0.0:
 *     - Add h264 encoder and decoder support
 *     v1.0.1:
 *     - Changed the IDF dependencies from >= 5.3 to >= 4.4
 *     - Fixed the decoder without updating PTS and DTS
 */

/**
 * @brief  Get H.264 version string
 *
 * @return
 *       - ESP_H264_VERSION
 */
const char *esp_h264_get_version(void);

#ifdef __cplusplus
}
#endif
