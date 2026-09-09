/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <unity.h>
#include "esp_h264_hw_enc_test.h"
#include "esp_h264_sw_enc_test.h"
#include "esp_h264_sw_dec_test.h"

static int16_t res_width = 128;
static int16_t res_height = 128;

#if CONFIG_IDF_TARGET_ESP32P4

static int16_t res_width1 = 128;
static int16_t res_height1 = 128;

TEST_CASE("hw_enc_single_hw_enc_gop_test", "[esp_h264]")
{
    for (int16_t gop = 1; gop < 256; gop++) {
        esp_h264_enc_cfg_hw_t cfg = { 0 };
        cfg.gop = gop;
        cfg.fps = 30;
        cfg.res.width = res_width;
        cfg.res.height = res_height;
        cfg.rc.bitrate = cfg.res.width * cfg.res.height * cfg.fps / 20;
        cfg.rc.qp_min = 26;
        cfg.rc.qp_max = 26;
        cfg.pic_type = ESP_H264_RAW_FMT_O_UYY_E_VYY;
        TEST_ASSERT_EQUAL(ESP_H264_ERR_OK, single_hw_enc_process(cfg));
    }
}

TEST_CASE("hw_enc_single_hw_enc_fps_test", "[esp_h264]")
{
    for (int16_t fps = 1; fps < 256; fps++) {
        esp_h264_enc_cfg_hw_t cfg = { 0 };
        cfg.gop = 5;
        cfg.fps = fps;
        cfg.res.width = res_width;
        cfg.res.height = res_height;
        cfg.rc.bitrate = cfg.res.width * cfg.res.height * cfg.fps / 20;
        cfg.rc.qp_min = 26;
        cfg.rc.qp_max = 26;
        cfg.pic_type = ESP_H264_RAW_FMT_O_UYY_E_VYY;
        TEST_ASSERT_EQUAL(ESP_H264_ERR_OK, single_hw_enc_process(cfg));
    }
}

TEST_CASE("hw_enc_single_hw_enc_qp_test", "[esp_h264]")
{
    for (int16_t qp = 0; qp <= 51; qp++) {
        esp_h264_enc_cfg_hw_t cfg = { 0 };
        cfg.gop = 5;
        cfg.fps = 30;
        cfg.res.width = res_width;
        cfg.res.height = res_height;
        cfg.rc.bitrate = cfg.res.width * cfg.res.height * cfg.fps / 20;
        cfg.rc.qp_min = qp < 0 ? 0 : qp;
        cfg.rc.qp_max = (cfg.rc.qp_min + 1) > 51 ? 51 : (cfg.rc.qp_min + 1);
        cfg.pic_type = ESP_H264_RAW_FMT_O_UYY_E_VYY;
        TEST_ASSERT_EQUAL(ESP_H264_ERR_OK, single_hw_enc_process(cfg));
    }
}

TEST_CASE("hw_enc_dual_hw_enc_gop_test", "[esp_h264]")
{
    for (int16_t gop = 1; gop < 256; gop++) {
        esp_h264_enc_cfg_dual_hw_t cfg;
        cfg.cfg0.gop = gop;
        cfg.cfg0.fps = 30;
        cfg.cfg0.res.width = res_width;
        cfg.cfg0.res.height = res_height;
        cfg.cfg0.rc.bitrate = cfg.cfg0.res.width * cfg.cfg0.res.height * cfg.cfg0.fps / 20;
        cfg.cfg0.rc.qp_min = 26;
        cfg.cfg0.rc.qp_max = 26;
        cfg.cfg0.pic_type = ESP_H264_RAW_FMT_O_UYY_E_VYY;

        cfg.cfg1.gop = gop;
        cfg.cfg1.fps = 30;
        cfg.cfg1.res.width = res_width1;
        cfg.cfg1.res.height = res_height1;
        cfg.cfg1.rc.bitrate = cfg.cfg1.res.width * cfg.cfg1.res.height * cfg.cfg1.fps / 20;
        cfg.cfg1.rc.qp_min = 26;
        cfg.cfg1.rc.qp_max = 26;
        cfg.cfg1.pic_type = ESP_H264_RAW_FMT_O_UYY_E_VYY;
        TEST_ASSERT_EQUAL(ESP_H264_ERR_OK, dual_hw_enc_process(cfg));
    }
}

TEST_CASE("hw_enc_dual_hw_enc_fps_test", "[esp_h264]")
{
    for (int16_t fps = 1; fps < 256; fps++) {
        esp_h264_enc_cfg_dual_hw_t cfg;
        cfg.cfg0.gop = 5;
        cfg.cfg0.fps = fps;
        cfg.cfg0.res.width = res_width;
        cfg.cfg0.res.height = res_height;
        cfg.cfg0.rc.bitrate = cfg.cfg0.res.width * cfg.cfg0.res.height * cfg.cfg0.fps / 20;
        cfg.cfg0.rc.qp_min = 26;
        cfg.cfg0.rc.qp_max = 26;
        cfg.cfg0.pic_type = ESP_H264_RAW_FMT_O_UYY_E_VYY;

        cfg.cfg1.gop = 5;
        cfg.cfg1.fps = ((fps + 5) % 256) == 0 ? 1 : ((fps + 5) % 256);
        cfg.cfg1.res.width = res_width1;
        cfg.cfg1.res.height = res_height1;
        cfg.cfg1.rc.bitrate = cfg.cfg1.res.width * cfg.cfg1.res.height * cfg.cfg1.fps / 20;
        cfg.cfg1.rc.qp_min = 26;
        cfg.cfg1.rc.qp_max = 26;
        cfg.cfg1.pic_type = ESP_H264_RAW_FMT_O_UYY_E_VYY;
        TEST_ASSERT_EQUAL(ESP_H264_ERR_OK, dual_hw_enc_process(cfg));
    }
}

TEST_CASE("hw_enc_dual_hw_enc_qp_test", "[esp_h264]")
{
    for (int16_t qp = 0; qp <= 51; qp++) {
        esp_h264_enc_cfg_dual_hw_t cfg;
        cfg.cfg0.gop = 5;
        cfg.cfg0.fps = 30;
        cfg.cfg0.res.width = res_width;
        cfg.cfg0.res.height = res_height;
        cfg.cfg0.rc.bitrate = cfg.cfg0.res.width * cfg.cfg0.res.height * cfg.cfg0.fps / 20;
        cfg.cfg0.rc.qp_min = qp < 0 ? 0 : qp;
        cfg.cfg0.rc.qp_max = (cfg.cfg0.rc.qp_min + 1) > 51 ? 51 : (cfg.cfg0.rc.qp_min + 1);
        cfg.cfg0.pic_type = ESP_H264_RAW_FMT_O_UYY_E_VYY;

        cfg.cfg1.gop = 5;
        cfg.cfg1.fps = 30;
        cfg.cfg1.res.width = res_width1;
        cfg.cfg1.res.height = res_height1;
        cfg.cfg1.rc.bitrate = cfg.cfg1.res.width * cfg.cfg1.res.height * cfg.cfg1.fps / 20;
        cfg.cfg1.rc.qp_min = qp < 0 ? 0 : (qp + 1) % 51;
        cfg.cfg1.rc.qp_max = (cfg.cfg1.rc.qp_min + 1) > 51 ? 51 : (cfg.cfg1.rc.qp_min + 1);
        cfg.cfg1.pic_type = ESP_H264_RAW_FMT_O_UYY_E_VYY;
        TEST_ASSERT_EQUAL(ESP_H264_ERR_OK, dual_hw_enc_process(cfg));
    }
}

TEST_CASE("hw_enc_set_get_param_single_hw_enc_thread_test", "[esp_h264]")
{
    esp_h264_enc_cfg_hw_t cfg = { 0 };
    cfg.gop = 5;
    cfg.fps = 30;
    cfg.res.width = res_width;
    cfg.res.height = res_height;
    cfg.rc.bitrate = cfg.res.width * cfg.res.height * cfg.fps / 20;
    cfg.rc.qp_min = 26;
    cfg.rc.qp_max = 26;
    cfg.pic_type = ESP_H264_RAW_FMT_O_UYY_E_VYY;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_OK, single_hw_enc_thread_test(cfg));
}

TEST_CASE("hw_enc_set_get_param_dual_hw_enc_thread_test", "[esp_h264]")
{
    esp_h264_enc_cfg_dual_hw_t cfg;
    cfg.cfg0.gop = 5;
    cfg.cfg0.fps = 30;
    cfg.cfg0.res.width = res_width;
    cfg.cfg0.res.height = res_height;
    cfg.cfg0.rc.bitrate = cfg.cfg0.res.width * cfg.cfg0.res.height * cfg.cfg0.fps / 20;
    cfg.cfg0.rc.qp_min = 26;
    cfg.cfg0.rc.qp_max = 26;
    cfg.cfg0.pic_type = ESP_H264_RAW_FMT_O_UYY_E_VYY;

    cfg.cfg1.gop = 5;
    cfg.cfg1.fps = 30;
    cfg.cfg1.res.width = res_width1;
    cfg.cfg1.res.height = res_height1;
    cfg.cfg1.rc.bitrate = cfg.cfg1.res.width * cfg.cfg1.res.height * cfg.cfg1.fps / 20;
    cfg.cfg1.rc.qp_min = 26;
    cfg.cfg1.rc.qp_max = 26;
    cfg.cfg1.pic_type = ESP_H264_RAW_FMT_O_UYY_E_VYY;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_OK, dual_hw_enc_thread_test(cfg));
}

TEST_CASE("hw_enc_set_get_param_single_hw_enc_roi_cfg_test", "[esp_h264]")
{
    esp_h264_enc_cfg_hw_t cfg = { 0 };
    cfg.gop = 5;
    cfg.fps = 30;
    cfg.res.width = res_width;
    cfg.res.height = res_height;
    cfg.rc.bitrate = cfg.res.width * cfg.res.height * cfg.fps / 20;
    cfg.rc.qp_min = 26;
    cfg.rc.qp_max = 26;
    cfg.pic_type = ESP_H264_RAW_FMT_O_UYY_E_VYY;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_OK, single_hw_enc_roi_cfg_test(cfg));
}

TEST_CASE("hw_enc_set_get_param_dual_hw_enc_roi_cfg_test", "[esp_h264]")
{
    esp_h264_enc_cfg_dual_hw_t cfg;
    cfg.cfg0.gop = 5;
    cfg.cfg0.fps = 30;
    cfg.cfg0.res.width = res_width;
    cfg.cfg0.res.height = res_height;
    cfg.cfg0.rc.bitrate = cfg.cfg0.res.width * cfg.cfg0.res.height * cfg.cfg0.fps / 20;
    cfg.cfg0.rc.qp_min = 26;
    cfg.cfg0.rc.qp_max = 26;
    cfg.cfg0.pic_type = ESP_H264_RAW_FMT_O_UYY_E_VYY;

    cfg.cfg1.gop = 5;
    cfg.cfg1.fps = 30;
    cfg.cfg1.res.width = res_width1;
    cfg.cfg1.res.height = res_height1;
    cfg.cfg1.rc.bitrate = cfg.cfg1.res.width * cfg.cfg1.res.height * cfg.cfg1.fps / 20;
    cfg.cfg1.rc.qp_min = 26;
    cfg.cfg1.rc.qp_max = 26;
    cfg.cfg1.pic_type = ESP_H264_RAW_FMT_O_UYY_E_VYY;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_OK, dual_hw_enc_roi_cfg_test(cfg));
}

TEST_CASE("hw_enc_set_get_param_single_hw_enc_roi_reg_test", "[esp_h264]")
{
    esp_h264_enc_cfg_hw_t cfg = { 0 };
    cfg.gop = 5;
    cfg.fps = 30;
    cfg.res.width = res_width;
    cfg.res.height = res_height;
    cfg.rc.bitrate = cfg.res.width * cfg.res.height * cfg.fps / 20;
    cfg.rc.qp_min = 26;
    cfg.rc.qp_max = 26;
    cfg.pic_type = ESP_H264_RAW_FMT_O_UYY_E_VYY;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_OK, single_hw_enc_roi_reg_test(cfg));
}

TEST_CASE("hw_enc_set_get_param_dual_hw_enc_roi_reg_test", "[esp_h264]")
{
    esp_h264_enc_cfg_dual_hw_t cfg;
    cfg.cfg0.gop = 5;
    cfg.cfg0.fps = 30;
    cfg.cfg0.res.width = res_width;
    cfg.cfg0.res.height = res_height;
    cfg.cfg0.rc.bitrate = cfg.cfg0.res.width * cfg.cfg0.res.height * cfg.cfg0.fps / 20;
    cfg.cfg0.rc.qp_min = 26;
    cfg.cfg0.rc.qp_max = 26;
    cfg.cfg0.pic_type = ESP_H264_RAW_FMT_O_UYY_E_VYY;

    cfg.cfg1.gop = 5;
    cfg.cfg1.fps = 30;
    cfg.cfg1.res.width = res_width1;
    cfg.cfg1.res.height = res_height1;
    cfg.cfg1.rc.bitrate = cfg.cfg1.res.width * cfg.cfg1.res.height * cfg.cfg1.fps / 20;
    cfg.cfg1.rc.qp_min = 26;
    cfg.cfg1.rc.qp_max = 26;
    cfg.cfg1.pic_type = ESP_H264_RAW_FMT_O_UYY_E_VYY;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_OK, dual_hw_enc_roi_reg_test(cfg));
}

TEST_CASE("hw_enc_set_get_param_single_hw_enc_mv_pkt_test", "[esp_h264]")
{
    esp_h264_enc_cfg_hw_t cfg = { 0 };
    cfg.gop = 5;
    cfg.fps = 30;
    cfg.res.width = res_width;
    cfg.res.height = res_height;
    cfg.rc.bitrate = cfg.res.width * cfg.res.height * cfg.fps / 20;
    cfg.rc.qp_min = 26;
    cfg.rc.qp_max = 26;
    cfg.pic_type = ESP_H264_RAW_FMT_O_UYY_E_VYY;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_OK, single_hw_enc_mv_pkt_test(cfg));
}

TEST_CASE("hw_enc_set_get_param_dual_hw_enc_mv_pkt_test", "[esp_h264]")
{
    esp_h264_enc_cfg_dual_hw_t cfg;
    cfg.cfg0.gop = 5;
    cfg.cfg0.fps = 30;
    cfg.cfg0.res.width = res_width;
    cfg.cfg0.res.height = res_height;
    cfg.cfg0.rc.bitrate = cfg.cfg0.res.width * cfg.cfg0.res.height * cfg.cfg0.fps / 20;
    cfg.cfg0.rc.qp_min = 26;
    cfg.cfg0.rc.qp_max = 26;
    cfg.cfg0.pic_type = ESP_H264_RAW_FMT_O_UYY_E_VYY;

    cfg.cfg1.gop = 5;
    cfg.cfg1.fps = 30;
    cfg.cfg1.res.width = res_width1;
    cfg.cfg1.res.height = res_height1;
    cfg.cfg1.rc.bitrate = cfg.cfg1.res.width * cfg.cfg1.res.height * cfg.cfg1.fps / 20;
    cfg.cfg1.rc.qp_min = 26;
    cfg.cfg1.rc.qp_max = 26;
    cfg.cfg1.pic_type = ESP_H264_RAW_FMT_O_UYY_E_VYY;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_OK, dual_hw_enc_mv_pkt_test(cfg));
}

/* error test */
TEST_CASE("hw_enc_error_test", "[esp_h264]")
{
    esp_h264_enc_cfg_hw_t cfg = { 0 };
    esp_h264_enc_handle_t enc = NULL;
    cfg.gop = 255;
    cfg.fps = 30;
    cfg.res.width = res_width;
    cfg.res.height = res_height;
    cfg.rc.bitrate = cfg.res.width * cfg.res.height * cfg.fps / 20;
    cfg.rc.qp_min = 26;
    cfg.rc.qp_max = 26;
    cfg.pic_type = ESP_H264_RAW_FMT_O_UYY_E_VYY;

    /* cfg is NULL*/
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_hw_new(NULL, &enc));

    /* enc handle is NULL*/
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_hw_new(&cfg, NULL));

    /* pic_type isn't ESP_H264_RAW_FMT_O_UYY_E_VYY */
    cfg.pic_type = ESP_H264_RAW_FMT_I420;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_hw_new(&cfg, &enc));
    cfg.pic_type = ESP_H264_RAW_FMT_O_UYY_E_VYY;

    /* width is greater than 1920 */
    cfg.res.width = 2000;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_hw_new(&cfg, &enc));

    /* width is less than 64 */
    cfg.res.width = 48;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_hw_new(&cfg, &enc));
    cfg.res.width = 128;

    /* height is greater than 2048 */
    cfg.res.height = 3092;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_hw_new(&cfg, &enc));

    /* height is less than 80 */
    cfg.res.height = 48;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_hw_new(&cfg, &enc));
    cfg.res.height = res_height;

    /* qp_min is greater than 51 */
    cfg.rc.qp_min = 52;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_hw_new(&cfg, &enc));
    cfg.rc.qp_min = 26;

    /* qp_max is greater than 51 */
    cfg.rc.qp_max = 52;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_hw_new(&cfg, &enc));

    /* qp_min is greater than qp_max */
    cfg.rc.qp_max = cfg.rc.qp_min - 1;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_hw_new(&cfg, &enc));
    cfg.rc.qp_max = 26;

    /* GOP is 0 */
    cfg.gop = 0;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_hw_new(&cfg, &enc));
    cfg.gop = 30;

    /* FPS is 0 */
    cfg.fps = 0;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_hw_new(&cfg, &enc));
    cfg.fps = 5;

    TEST_ASSERT_EQUAL(ESP_H264_ERR_OK, esp_h264_enc_hw_new(&cfg, &enc));

    esp_h264_enc_param_hw_handle_t param_hd = NULL;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_OK, esp_h264_enc_hw_get_param_hd(enc, &param_hd));

    /* get_resolution: param_hd is NULL */
    esp_h264_resolution_t res;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_get_resolution(NULL, &res));

    /* get_resolution: resolution is NULL */
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_get_resolution(&param_hd->base, NULL));

    /* set_fps: param_hd is NULL */
    uint8_t fps;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_set_fps(NULL, fps));

    /* get_fps: param_hd is NULL */
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_get_fps(NULL, &fps));

    /* get_fps: fps is NULL */
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_get_fps(&param_hd->base, NULL));

    /* set_gop: param_hd is NULL */
    uint8_t gop;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_set_gop(NULL, gop));

    /* get_gop: param_hd is NULL */
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_get_gop(NULL, &gop));

    /* get_gop: gop is NULL */
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_get_gop(&param_hd->base, NULL));

    /* set_bitrate: param_hd is NULL */
    esp_h264_enc_rc_t rc = {
        .qp_min = 26,
        .qp_max = 27,
    };
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_set_bitrate(NULL, rc.bitrate));

    /* get_bitrate: param_hd is NULL  */
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_get_bitrate(NULL, &rc.bitrate));

    /* get_bitrate: bitrate is NULL  */
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_get_bitrate(&param_hd->base, NULL));

    /* cfg_roi: param_hd is NULL  */
    esp_h264_enc_roi_cfg_t roi_cfg;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_hw_cfg_roi(NULL, roi_cfg));

    /* cfg_roi: mode is invalid */
    roi_cfg.roi_mode = ESP_H264_ROI_MODE_INVALID;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_hw_cfg_roi(param_hd, roi_cfg));
    roi_cfg.roi_mode = ESP_H264_ROI_MODE_DELTA_QP;

    /* cfg_roi: none_roi_delta_qp is greater than 51 */
    roi_cfg.none_roi_delta_qp = 52;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_hw_cfg_roi(param_hd, roi_cfg));

    /* cfg_roi: none_roi_delta_qp is less than -51 */
    roi_cfg.none_roi_delta_qp = -52;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_hw_cfg_roi(param_hd, roi_cfg));

    /* get_roi_cfg_info: param_hd is NULL  */
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_hw_get_roi_cfg_info(NULL, &roi_cfg));

    /* get_roi_cfg_info: ROI is invalid */
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_hw_get_roi_cfg_info(param_hd, NULL));

    /* set_roi_region: param_hd is NULL  */
    esp_h264_enc_roi_reg_t roi_reg = { 0 };
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_hw_set_roi_region(NULL, roi_reg));

    /* set_roi_region: qp is greater than 51  */
    roi_reg.qp = 52;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_hw_set_roi_region(param_hd, roi_reg));
    roi_reg.qp = 50;

    /* set_roi_region: ESP_H264_ROI_MODE_DELTA_QP, qp is less than -51  */
    roi_reg.qp = -52;
    roi_cfg.roi_mode = ESP_H264_ROI_MODE_DELTA_QP;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_hw_cfg_roi(param_hd, roi_cfg));
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_hw_set_roi_region(param_hd, roi_reg));
    /* set_roi_region: ESP_H264_ROI_MODE_FIX_QP, qp is less than -51  */
    roi_reg.qp = -1;
    roi_cfg.roi_mode = ESP_H264_ROI_MODE_FIX_QP;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_hw_cfg_roi(param_hd, roi_cfg));
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_hw_set_roi_region(param_hd, roi_reg));
    roi_reg.qp = 50;

    /* set_roi_region: reg_idx is greater than 8  */
    roi_reg.reg_idx = 9;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_hw_set_roi_region(param_hd, roi_reg));

    /* get_roi_region: param_hd is NULL  */
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_hw_get_roi_region(NULL, &roi_reg));

    /* get_roi_region: ROI region is NULL  */
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_hw_get_roi_region(param_hd, NULL));

    /* get_roi_region: reg_idx is greater than 8  */
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_hw_get_roi_region(param_hd, &roi_reg));

    /* cfg_mv: param_hd is NULL  */
    esp_h264_enc_mv_cfg_t mv_cfg;
    mv_cfg.mv_mode = ESP_H264_MVM_MODE_P16X16;
    mv_cfg.mv_fmt = ESP_H264_MVM_FMT_PART;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_hw_cfg_mv(NULL, mv_cfg));

    /* cfg_mv: mode is invalid */
    mv_cfg.mv_mode = ESP_H264_MVM_MODE_INVALID;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_hw_cfg_mv(param_hd, mv_cfg));
    mv_cfg.mv_mode = ESP_H264_MVM_MODE_P16X16;

    /* cfg_mv: mv_fmt is invalid */
    mv_cfg.mv_fmt = ESP_H264_MVM_FMT_INVALID;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_hw_cfg_mv(param_hd, mv_cfg));
    mv_cfg.mv_fmt = ESP_H264_MVM_FMT_PART;

    /* get_mv_cfg_info: param_hd is NULL  */
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_hw_get_mv_cfg_info(NULL, &mv_cfg));

    /* get_mv_cfg_info: MV configure is NULL  */
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_hw_get_mv_cfg_info(param_hd, NULL));

    /* set_mv_pkt: param_hd is NULL  */
    esp_h264_enc_mvm_pkt_t mv_pkt;
    esp_h264_enc_mv_data_t mv_data;
    mv_pkt.len = 12;
    mv_pkt.data = &mv_data;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_hw_set_mv_pkt(NULL, mv_pkt));
    /* set_mv_pkt: buffer is NULL  */
    mv_pkt.data = NULL;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_hw_set_mv_pkt(param_hd, mv_pkt));
    /* set_mv_pkt: length is 0  */
    mv_pkt.len = 0;
    mv_pkt.data = &mv_data;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_hw_set_mv_pkt(param_hd, mv_pkt));

    /* get_mv_data_len: param_hd is NULL  */
    uint32_t length;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_hw_get_mv_data_len(NULL, &length));

    /* get_mv_data_len: length is NULL  */
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_hw_get_mv_data_len(param_hd, NULL));

    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_open(NULL));

    TEST_ASSERT_EQUAL(ESP_H264_ERR_OK, esp_h264_enc_open(enc));

    esp_h264_enc_in_frame_t in_frame = {
        .raw_data.buffer = (uint8_t *)1,
        .raw_data.len = 1,
    };
    esp_h264_enc_out_frame_t out_frame = {
        .raw_data.buffer = (uint8_t *)1,
        .raw_data.len = 1,
    };

    /* enc is NULL */
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_process(NULL, &in_frame, &out_frame));
    /* in_frame is NULL */
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_process(enc, NULL, &out_frame));
    /* in_frame.raw_data.buffer is NULL */
    in_frame.raw_data.buffer = 0;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_process(enc, &in_frame, &out_frame));
    in_frame.raw_data.buffer = (uint8_t *)1;

    /* out_frame is NULL */
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_process(enc, &in_frame, NULL));
    /* out_frame.raw_data.buffer is NULL */
    out_frame.raw_data.buffer = 0;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_process(enc, &in_frame, &out_frame));
    out_frame.raw_data.buffer = (uint8_t *)1;

    /* enc is NULL */
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_close(NULL));

    /* close enc */
    TEST_ASSERT_EQUAL(ESP_H264_ERR_OK, esp_h264_enc_close(enc));

    /* enc is NULL */
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_del(NULL));

    /* delete enc */
    TEST_ASSERT_EQUAL(ESP_H264_ERR_OK, esp_h264_enc_del(enc));

    esp_h264_enc_cfg_dual_hw_t cfg_dual;
    esp_h264_enc_dual_handle_t enc_dual = NULL;
    cfg_dual.cfg0.gop = 5;
    cfg_dual.cfg0.fps = 30;
    cfg_dual.cfg0.res.width = 128;
    cfg_dual.cfg0.res.height = 128;
    cfg_dual.cfg0.rc.bitrate = cfg_dual.cfg0.res.width * cfg_dual.cfg0.res.height * cfg_dual.cfg0.fps / 20;
    cfg_dual.cfg0.rc.qp_min = 26;
    cfg_dual.cfg0.rc.qp_max = 26;
    cfg_dual.cfg0.pic_type = ESP_H264_RAW_FMT_O_UYY_E_VYY;

    cfg_dual.cfg1.gop = 5;
    cfg_dual.cfg1.fps = 30;
    cfg_dual.cfg1.res.width = 128;
    cfg_dual.cfg1.res.height = 128;
    cfg_dual.cfg1.rc.bitrate = cfg_dual.cfg1.res.width * cfg_dual.cfg1.res.height * cfg_dual.cfg1.fps / 20;
    cfg_dual.cfg1.rc.qp_min = 26;
    cfg_dual.cfg1.rc.qp_max = 26;
    cfg_dual.cfg1.pic_type = ESP_H264_RAW_FMT_O_UYY_E_VYY;

    /* cfg is NULL*/
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_dual_hw_new(NULL, &enc_dual));

    /* enc handle is NULL*/
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_dual_hw_new(&cfg_dual, NULL));

    /* pic_type isn't ESP_H264_RAW_FMT_O_UYY_E_VYY */
    cfg_dual.cfg0.pic_type = ESP_H264_RAW_FMT_I420;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_dual_hw_new(&cfg_dual, &enc_dual));
    cfg_dual.cfg0.pic_type = ESP_H264_RAW_FMT_O_UYY_E_VYY;

    cfg_dual.cfg1.pic_type = ESP_H264_RAW_FMT_I420;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_dual_hw_new(&cfg_dual, &enc_dual));
    cfg_dual.cfg1.pic_type = ESP_H264_RAW_FMT_O_UYY_E_VYY;

    /* width is greater than 1920 */
    cfg_dual.cfg0.res.width = 2000;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_dual_hw_new(&cfg_dual, &enc_dual));
    cfg_dual.cfg0.res.width = 128;

    cfg_dual.cfg1.res.width = 2000;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_dual_hw_new(&cfg_dual, &enc_dual));
    cfg_dual.cfg1.res.width = 128;

    /* width is less than 64 */
    cfg_dual.cfg0.res.width = 48;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_dual_hw_new(&cfg_dual, &enc_dual));
    cfg_dual.cfg0.res.width = 128;

    cfg_dual.cfg1.res.width = 48;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_dual_hw_new(&cfg_dual, &enc_dual));
    cfg_dual.cfg1.res.width = 128;

    /* height is greater than 2048 */
    cfg_dual.cfg0.res.height = 3092;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_dual_hw_new(&cfg_dual, &enc_dual));
    cfg_dual.cfg0.res.height = 128;

    cfg_dual.cfg1.res.height = 3092;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_dual_hw_new(&cfg_dual, &enc_dual));
    cfg_dual.cfg1.res.height = 128;

    /* height is less than 80 */
    cfg_dual.cfg0.res.height = 48;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_dual_hw_new(&cfg_dual, &enc_dual));
    cfg_dual.cfg0.res.height = 128;

    /* height is less than 80 */
    cfg_dual.cfg1.res.height = 48;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_dual_hw_new(&cfg_dual, &enc_dual));
    cfg_dual.cfg1.res.height = 128;

    /* qp_min is greater than 51 */
    cfg_dual.cfg0.rc.qp_min = 52;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_dual_hw_new(&cfg_dual, &enc_dual));
    cfg_dual.cfg0.rc.qp_min = 26;

    cfg_dual.cfg1.rc.qp_min = 52;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_dual_hw_new(&cfg_dual, &enc_dual));
    cfg_dual.cfg1.rc.qp_min = 26;

    /* qp_max is greater than 51 */
    cfg_dual.cfg0.rc.qp_max = 52;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_dual_hw_new(&cfg_dual, &enc_dual));
    cfg_dual.cfg0.rc.qp_max = 26;

    cfg_dual.cfg1.rc.qp_max = 52;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_dual_hw_new(&cfg_dual, &enc_dual));
    cfg_dual.cfg1.rc.qp_max = 26;

    /* qp_min is greater than qp_max */
    cfg_dual.cfg0.rc.qp_max = cfg_dual.cfg0.rc.qp_min - 1;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_dual_hw_new(&cfg_dual, &enc_dual));
    cfg_dual.cfg0.rc.qp_max = 26;

    cfg_dual.cfg1.rc.qp_max = cfg_dual.cfg1.rc.qp_min - 1;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_dual_hw_new(&cfg_dual, &enc_dual));
    cfg_dual.cfg1.rc.qp_max = 26;

    /* GOP is 0 */
    cfg_dual.cfg0.gop = 0;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_dual_hw_new(&cfg_dual, &enc_dual));
    cfg_dual.cfg0.gop = 30;

    cfg_dual.cfg1.gop = 0;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_dual_hw_new(&cfg_dual, &enc_dual));
    cfg_dual.cfg1.gop = 30;

    /* FPS is 0 */
    cfg_dual.cfg0.fps = 0;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_dual_hw_new(&cfg_dual, &enc_dual));
    cfg_dual.cfg0.fps = 30;

    cfg_dual.cfg1.fps = 0;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_dual_hw_new(&cfg_dual, &enc_dual));
    cfg_dual.cfg1.fps = 30;

    TEST_ASSERT_EQUAL(ESP_H264_ERR_OK, esp_h264_enc_dual_hw_new(&cfg_dual, &enc_dual));
    esp_h264_enc_param_hw_handle_t param_hd0 = NULL;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_OK, esp_h264_enc_dual_hw_get_param_hd0(enc_dual, &param_hd0));

    esp_h264_enc_param_hw_handle_t param_hd1 = NULL;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_OK, esp_h264_enc_dual_hw_get_param_hd1(enc_dual, &param_hd1));

    /* enc is NULL */
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_dual_open(NULL));

    /* open enc */
    TEST_ASSERT_EQUAL(ESP_H264_ERR_OK, esp_h264_enc_dual_open(enc_dual));

    esp_h264_enc_in_frame_t in_frame1 = {
        .raw_data.buffer = (uint8_t *)1,
        .raw_data.len = 1,
    };
    esp_h264_enc_out_frame_t out_frame1 = {
        .raw_data.buffer = (uint8_t *)1,
        .raw_data.len = 1,
    };
    esp_h264_enc_in_frame_t *in_frame_dual[2] = { &in_frame, &in_frame1 };
    esp_h264_enc_out_frame_t *out_frame_dual[2] = { &out_frame, &out_frame1 };

    /* enc_dual is NULL */
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_dual_process(NULL, in_frame_dual, out_frame_dual));

    /* in_frame is NULL */
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_dual_process(enc_dual, NULL, out_frame_dual));
    in_frame_dual[0] = 0;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_dual_process(enc_dual, in_frame_dual, out_frame_dual));
    in_frame_dual[0] = &in_frame;

    in_frame_dual[1] = 0;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_dual_process(enc_dual, in_frame_dual, out_frame_dual));
    in_frame_dual[1] = &in_frame1;

    /* in_frame.raw_data.buffer is NULL */
    in_frame.raw_data.buffer = 0;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_dual_process(enc_dual, in_frame_dual, out_frame_dual));
    in_frame.raw_data.buffer = (uint8_t *)1;

    in_frame1.raw_data.buffer = 0;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_dual_process(enc_dual, in_frame_dual, out_frame_dual));
    in_frame1.raw_data.buffer = (uint8_t *)1;

    /* out_frame is NULL */
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_dual_process(enc_dual, in_frame_dual, NULL));

    out_frame_dual[0] = 0;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_dual_process(enc_dual, in_frame_dual, out_frame_dual));
    out_frame_dual[0] = &out_frame;

    out_frame_dual[1] = 0;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_dual_process(enc_dual, in_frame_dual, out_frame_dual));
    out_frame_dual[1] = &out_frame1;

    /* out_frame.raw_data.buffer is NULL */
    out_frame.raw_data.buffer = 0;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_dual_process(enc_dual, in_frame_dual, out_frame_dual));
    out_frame.raw_data.buffer = (uint8_t *)1;

    out_frame1.raw_data.buffer = 0;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_dual_process(enc_dual, in_frame_dual, out_frame_dual));
    out_frame1.raw_data.buffer = (uint8_t *)1;

    /* enc is NULL */
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_dual_close(NULL));

    /* close enc */
    TEST_ASSERT_EQUAL(ESP_H264_ERR_OK, esp_h264_enc_dual_close(enc_dual));

    /* enc is NULL */
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_dual_del(NULL));

    /* delete enc */
    TEST_ASSERT_EQUAL(ESP_H264_ERR_OK, esp_h264_enc_dual_del(enc_dual));
}
#endif //CONFIG_IDF_TARGET_ESP32P4

/* error test */
TEST_CASE("sw_dec_error_test", "[esp_h264]")
{
    esp_h264_dec_cfg_sw_t cfg;
    cfg.pic_type = ESP_H264_RAW_FMT_I420;
    esp_h264_dec_handle_t dec = NULL;

    /* cfg is NULL*/
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_dec_sw_new(NULL, &dec));

    /* dec handle is NULL*/
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_dec_sw_new(&cfg, NULL));

    /* pic_type isn't ESP_H264_RAW_FMT_I420*/
    cfg.pic_type = ESP_H264_RAW_FMT_O_UYY_E_VYY;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_dec_sw_new(&cfg, NULL));

    cfg.pic_type = ESP_H264_RAW_FMT_I420;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_OK, esp_h264_dec_sw_new(&cfg, &dec));

    esp_h264_dec_param_handle_t param_hd = NULL;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_OK, esp_h264_dec_sw_get_param_hd(dec, &param_hd));

    /* get_resolution: param_hd is NULL */
    esp_h264_resolution_t res;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_dec_get_resolution(NULL, &res));

    /* get_resolution: resolution is NULL */
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_dec_get_resolution(param_hd, NULL));

    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_dec_open(NULL));

    TEST_ASSERT_EQUAL(ESP_H264_ERR_OK, esp_h264_dec_open(dec));

    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_dec_close(NULL));

    TEST_ASSERT_EQUAL(ESP_H264_ERR_OK, esp_h264_dec_close(dec));

    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_dec_del(NULL));

    TEST_ASSERT_EQUAL(ESP_H264_ERR_OK, esp_h264_dec_del(dec));
}

/* configure test */
TEST_CASE("sw_dec_data_error_test", "[esp_h264]")
{
    uint8_t *yuv = NULL;
    esp_h264_dec_cfg_sw_t cfg;
    cfg.pic_type = ESP_H264_RAW_FMT_I420;

    //start code error
    uint32_t inbuf_len = 4;
    uint8_t inbuf[30] = {0x00, 0x00, 0x00, 0x02};
    TEST_ASSERT_EQUAL(ESP_H264_ERR_FAIL, single_sw_dec_process(cfg, inbuf, inbuf_len, yuv));

    //forbidden_zero_bit is not 0
    inbuf_len = 5;
    inbuf[3] = 0x01;
    inbuf[4] = 0xFF;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_FAIL, single_sw_dec_process(cfg, inbuf, inbuf_len, yuv));

    //nal_uint_type is error
    inbuf_len = 5;
    inbuf[3] = 0x01;
    inbuf[4] = 0x63;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_FAIL, single_sw_dec_process(cfg, inbuf, inbuf_len, yuv));

    //profile id is error
    inbuf_len = 6;
    inbuf[3] = 0x01;
    inbuf[4] = 0x67;
    inbuf[5] = 0x43;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_FAIL, single_sw_dec_process(cfg, inbuf, inbuf_len, yuv));

    //level id is error
    inbuf_len = 7;
    inbuf[5] = 0x42;
    inbuf[6] = 0xff;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_FAIL, single_sw_dec_process(cfg, inbuf, inbuf_len, yuv));

    //SPS ID is greater than 31
    inbuf_len = 8;
    inbuf[6] = 0xc0;
    inbuf[7] = 0xff;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_FAIL,  single_sw_dec_process(cfg, inbuf, inbuf_len, yuv));

    //SPS error
    inbuf_len = 10;
    inbuf[7] = 0x0b;
    inbuf[8] = 0x8c;
    inbuf[9] = 0x8d;
    inbuf[10] = 0x4f;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_FAIL, single_sw_dec_process(cfg, inbuf, inbuf_len, yuv));

    //entropy_coding_mode_flag error
    inbuf_len = 30;
    uint8_t sps[50] = {0x00, 0x00, 0x00, 0x01, 0x67, 0x42, 0xc0, 0x0b, 0x8c, 0x8d, 0x41, 0x02, 0x24, 0x03, 0xc2, 0x21, 0x1a, 0x80, 0x00, 0x00, 0x00, 0x01, 0x68};
    sps[23] = 0xff;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_FAIL, single_sw_dec_process(cfg, sps, inbuf_len, yuv));

    //weighted_bipred_idc error
    inbuf_len = 30;
    sps[23] = 0xCE;
    sps[24] = 0xff;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_FAIL, single_sw_dec_process(cfg, sps, inbuf_len, yuv));

    printf("Slice header error testing. \n");
    //ACCESS UNIT BOUNDARY CHECK
    inbuf_len = 50;
    sps[24] = 0x3c;
    sps[25] = 0x80;
    sps[26] = 0x00;
    sps[27] = 0x00;
    sps[28] = 0x00;
    sps[29] = 0x01;
    sps[30] = 0x65;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_FAIL, single_sw_dec_process(cfg, sps, inbuf_len, yuv));

    //Slice data error
    sps[31] = 0xB8;
    sps[32] = 0x00;
    sps[33] = 0x00;
    sps[34] = 0x50;
    sps[35] = 0x8c;
    sps[36] = 0x56;
    sps[37] = 0x38;
    sps[38] = 0x00;
    sps[39] = 0x19;
    sps[40] = 0xff;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_FAIL, single_sw_dec_process(cfg, sps, inbuf_len, yuv));
}

TEST_CASE("sw_enc_set_get_param_single_thread_test", "[esp_h264]")
{
    esp_h264_enc_cfg_sw_t cfg = { 0 };
    cfg.gop = 5;
    cfg.fps = 30;
    cfg.res.width = res_width;
    cfg.res.height = res_height;
    cfg.rc.bitrate = cfg.res.width * cfg.res.height * cfg.fps / 20;
    cfg.rc.qp_min = 26;
    cfg.rc.qp_max = 26;
    cfg.pic_type = ESP_H264_RAW_FMT_I420;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_OK, single_sw_enc_thread_test(cfg));
}

/* error test */
TEST_CASE("sw_enc_error_test", "[esp_h264]")
{
    esp_h264_enc_cfg_sw_t cfg = { 0 };
    esp_h264_enc_handle_t enc = NULL;
    cfg.gop = 255;
    cfg.fps = 30;
    cfg.res.width = res_width;
    cfg.res.height = res_height;
    cfg.rc.bitrate = cfg.res.width * cfg.res.height * cfg.fps / 20;
    cfg.rc.qp_min = 26;
    cfg.rc.qp_max = 26;
    cfg.pic_type = ESP_H264_RAW_FMT_I420;

    /* cfg is NULL*/
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_sw_new(NULL, &enc));

    /* enc handle is NULL*/
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_sw_new(&cfg, NULL));

    /* pic_type is ESP_H264_RAW_FMT_O_UYY_E_VYY */
    cfg.pic_type = ESP_H264_RAW_FMT_O_UYY_E_VYY;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_sw_new(&cfg, &enc));
    cfg.pic_type = ESP_H264_RAW_FMT_I420;

    /* width is less than 16 */
    cfg.res.width = 15;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_sw_new(&cfg, &enc));
    cfg.res.width = res_width;

    /* height is less than 15 */
    cfg.res.height = 15;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_sw_new(&cfg, &enc));
    cfg.res.height = res_height;

    /* qp_min is greater than 51 */
    cfg.rc.qp_min = 52;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_sw_new(&cfg, &enc));
    cfg.rc.qp_min = 26;

    /* qp_max is greater than 51 */
    cfg.rc.qp_max = 52;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_sw_new(&cfg, &enc));

    /* qp_min is greater than qp_max */
    cfg.rc.qp_max = cfg.rc.qp_min - 1;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_sw_new(&cfg, &enc));
    cfg.rc.qp_max = 26;

    /* GOP is 0 */
    cfg.gop = 0;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_sw_new(&cfg, &enc));
    cfg.gop = 255;

    /* FPS is 0 */
    cfg.fps = 0;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_sw_new(&cfg, &enc));
    cfg.fps = 30;

    TEST_ASSERT_EQUAL(ESP_H264_ERR_OK, esp_h264_enc_sw_new(&cfg, &enc));
    // printf("ret %d\n", esp_h264_enc_sw_new(&cfg, &enc));

    esp_h264_enc_param_handle_t param_hd = NULL;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_OK, esp_h264_enc_sw_get_param_hd(enc, &param_hd));

    /* get_resolution: param_hd is NULL */
    esp_h264_resolution_t res;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_get_resolution(NULL, &res));

    /* get_resolution: resolution is NULL */
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_get_resolution(param_hd, NULL));

    /* set_fps: param_hd is NULL */
    uint8_t fps;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_set_fps(NULL, fps));

    /* get_fps: param_hd is NULL */
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_get_fps(NULL, &fps));

    /* get_fps: fps is NULL */
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_get_fps(param_hd, NULL));

    /* set_gop: param_hd is NULL */
    uint8_t gop;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_set_gop(NULL, gop));

    /* get_gop: param_hd is NULL */
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_get_gop(NULL, &gop));

    /* get_gop: gop is NULL */
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_get_gop(param_hd, NULL));

    /* set_bitrate: param_hd is NULL */
    esp_h264_enc_rc_t rc = {
        .qp_min = 26,
        .qp_max = 27,
    };
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_set_bitrate(NULL, rc.bitrate));

    /* get_bitrate: param_hd is NULL  */
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_get_bitrate(NULL, &rc.bitrate));

    /* get_bitrate: bitrate is NULL  */
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_get_bitrate(param_hd, NULL));

    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_open(NULL));

    TEST_ASSERT_EQUAL(ESP_H264_ERR_OK, esp_h264_enc_open(enc));

    esp_h264_enc_in_frame_t in_frame = {
        .raw_data.buffer = (uint8_t *)1,
        .raw_data.len = 1,
    };
    esp_h264_enc_out_frame_t out_frame = {
        .raw_data.buffer = (uint8_t *)1,
        .raw_data.len = 1,
    };

    /* enc is NULL */
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_process(NULL, &in_frame, &out_frame));
    /* in_frame is NULL */
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_process(enc, NULL, &out_frame));
    /* in_frame.raw_data.buffer is NULL */
    in_frame.raw_data.buffer = 0;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_process(enc, &in_frame, &out_frame));
    in_frame.raw_data.buffer = (uint8_t *)1;

    /* out_frame is NULL */
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_process(enc, &in_frame, NULL));
    /* out_frame.raw_data.buffer is NULL */
    out_frame.raw_data.buffer = 0;
    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_process(enc, &in_frame, &out_frame));
    out_frame.raw_data.buffer = (uint8_t *)1;

    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_close(NULL));

    TEST_ASSERT_EQUAL(ESP_H264_ERR_OK, esp_h264_enc_close(enc));

    TEST_ASSERT_EQUAL(ESP_H264_ERR_ARG, esp_h264_enc_del(NULL));

    TEST_ASSERT_EQUAL(ESP_H264_ERR_OK, esp_h264_enc_del(enc));
}
