/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "esp_h264_alloc.h"
#include "esp_h264_enc_hw_param.h"

static const char *TAG = "H264_ENC.HW.SET";

#define ESP_H264_ROI_SUP_NUM    (8)
#define ESP_H264_REDUNDANT_BYTE (8 + 64)
#define SPS_PPS_BUF_SIZE        (100)

typedef struct esp_h264_param {
    esp_h264_enc_param_hw_t    hw_base;
    esp_h264_set_dev_t         device;
    uint8_t                    fps;
    uint8_t                    gop;
    esp_h264_rc_hd_t           rc_hd;
    uint8_t                    qp_init;
    uint32_t                   bitrate;
    uint16_t                   width;
    uint16_t                   height;
    uint8_t                    mb_width;
    uint8_t                    mb_height;
    uint8_t                   *nal_buf;
    uint8_t                    nal_buf_len;
    uint16_t                   nal_bit_len;
    uint8_t                   *mvm_buf;
    uint32_t                   mvm_buf_len;
    uint8_t                   *db;
    uint8_t                   *ref;
    h264_dma_desc_t           *dsc_ref;
    h264_dma_desc_t           *dsc_db[4];
    h264_dma_desc_t           *dsc_mvm;
    esp_h264_mutex_t           mutex;
} esp_h264_param_t;

/** Basic parameter configure */
static esp_h264_err_t get_res(esp_h264_enc_param_handle_t handle, esp_h264_resolution_t *res)
{
    esp_h264_enc_param_hw_handle_t param_base = __containerof(handle, esp_h264_enc_param_hw_t, base);
    esp_h264_param_t *param = __containerof(param_base, esp_h264_param_t, hw_base);
    res->width = param->width;
    res->height = param->height;
    return ESP_H264_ERR_OK;
}

static esp_h264_err_t set_fps(esp_h264_enc_param_handle_t handle, uint8_t fps)
{
    esp_h264_enc_param_hw_handle_t param_base = __containerof(handle, esp_h264_enc_param_hw_t, base);
    esp_h264_param_t *param = __containerof(param_base, esp_h264_param_t, hw_base);
    param->fps = fps;
    esp_h264_mutex_lock(param->mutex, ESP_H264_MAX_DELAY);
    if (param->rc_hd) {
        esp_h264_enc_hw_rc_set_bt_fps(param->rc_hd, param->bitrate, param->fps);
    }
    esp_h264_enc_set_sps(param->nal_buf, param->nal_buf_len, param->height, param->width, param->fps);
    esp_h264_mutex_unlock(param->mutex);
    return ESP_H264_ERR_OK;
}

static esp_h264_err_t get_fps(esp_h264_enc_param_handle_t handle, uint8_t *fps)
{
    esp_h264_enc_param_hw_handle_t param_base = __containerof(handle, esp_h264_enc_param_hw_t, base);
    esp_h264_param_t *param = __containerof(param_base, esp_h264_param_t, hw_base);
    *fps = param->fps;
    return ESP_H264_ERR_OK;
}

static esp_h264_err_t set_gop(esp_h264_enc_param_handle_t handle, uint8_t gop)
{
    esp_h264_enc_param_hw_handle_t param_base = __containerof(handle, esp_h264_enc_param_hw_t, base);
    esp_h264_param_t *param = __containerof(param_base, esp_h264_param_t, hw_base);
    param->gop = gop;
    return ESP_H264_ERR_OK;
}

static esp_h264_err_t get_gop(esp_h264_enc_param_handle_t handle, uint8_t *gop)
{
    esp_h264_enc_param_hw_handle_t param_base = __containerof(handle, esp_h264_enc_param_hw_t, base);
    esp_h264_param_t *param = __containerof(param_base, esp_h264_param_t, hw_base);
    *gop = param->gop;
    return ESP_H264_ERR_OK;
}

static esp_h264_err_t set_bitrate(esp_h264_enc_param_handle_t handle, uint32_t bitrate)
{
    esp_h264_enc_param_hw_handle_t param_base = __containerof(handle, esp_h264_enc_param_hw_t, base);
    esp_h264_param_t *param = __containerof(param_base, esp_h264_param_t, hw_base);
    param->bitrate = bitrate;
    if (param->rc_hd) {
        esp_h264_mutex_lock(param->mutex, ESP_H264_MAX_DELAY);
        esp_h264_enc_hw_rc_set_bt_fps(param->rc_hd, bitrate, param->fps);
        esp_h264_mutex_unlock(param->mutex);
    }
    return ESP_H264_ERR_OK;
}

static esp_h264_err_t get_bitrate(esp_h264_enc_param_handle_t handle, uint32_t *bitrate)
{
    esp_h264_enc_param_hw_handle_t param_base = __containerof(handle, esp_h264_enc_param_hw_t, base);
    esp_h264_param_t *param = __containerof(param_base, esp_h264_param_t, hw_base);
    *bitrate = param->bitrate;
    return ESP_H264_ERR_OK;
}

/** HW special parameter configure */
static esp_h264_err_t cfg_roi(esp_h264_enc_param_hw_handle_t handle, esp_h264_enc_roi_cfg_t cfg)
{
    esp_h264_param_t *param = __containerof(handle, esp_h264_param_t, hw_base);
    esp_h264_mutex_lock(param->mutex, ESP_H264_MAX_DELAY);
    h264_hal_set_roi_mode(param->device, (int8_t)cfg.roi_mode, cfg.none_roi_delta_qp);
    esp_h264_mutex_unlock(param->mutex);
    return ESP_H264_ERR_OK;
}

static esp_h264_err_t get_roi_cfg_info(esp_h264_enc_param_hw_handle_t handle, esp_h264_enc_roi_cfg_t *cfg)
{
    esp_h264_param_t *param = __containerof(handle, esp_h264_param_t, hw_base);
    cfg->roi_mode = ESP_H264_ROI_MODE_DISABLE;
    uint8_t roi_mode = 0;
    if (h264_hal_get_roi_mode(param->device, &roi_mode, &cfg->none_roi_delta_qp) == true) {
        cfg->roi_mode = roi_mode;
    }
    return ESP_H264_ERR_OK;
}

static esp_h264_err_t set_roi_reg(esp_h264_enc_param_hw_handle_t handle, esp_h264_enc_roi_reg_t roi_reg)
{
    esp_h264_param_t *param = __containerof(handle, esp_h264_param_t, hw_base);
    ESP_H264_RET_ON_FALSE(roi_reg.reg_idx < ESP_H264_ROI_SUP_NUM, ESP_H264_ERR_ARG, TAG, "Invalid `reg_idx` parameter");
    int16_t x_size = roi_reg.len_x + roi_reg.x;
    int16_t y_size = roi_reg.len_y + roi_reg.y;
    ESP_H264_RET_ON_FALSE(x_size <= param->mb_width, ESP_H264_ERR_ARG, TAG, "The sum of `len_x` and `x` is gather than `mb_width` ");
    ESP_H264_RET_ON_FALSE(y_size <= param->mb_height, ESP_H264_ERR_ARG, TAG, "The sum of `len_y` and `y` is gather than `mb_height` ");
    uint8_t roi_mode = 0;
    int8_t none_roi_delta_qp = 0;
    /* Check ROI function enable*/
    if (h264_hal_get_roi_mode(param->device, &roi_mode, &none_roi_delta_qp) == false) {
        /* When ROI function disable, the ROI region is meaningless. */
        ESP_H264_LOGE(TAG, "ROI mode is disable. Please enable ROI mode first.");
        return ESP_H264_ERR_ARG;
    }
    switch (roi_mode) {
    case ESP_H264_ROI_MODE_DELTA_QP:
        ESP_H264_RET_ON_FALSE((roi_reg.qp >= -ESP_H264_QP_MAX), ESP_H264_ERR_ARG, TAG, "In ESP_H264_ROI_MODE_DELTA_QP, ROI region QP less than -51");
        break;
    case ESP_H264_ROI_MODE_FIX_QP:
        ESP_H264_RET_ON_FALSE((roi_reg.qp >= 0), ESP_H264_ERR_ARG, TAG, "In ESP_H264_ROI_MODE_FIX_QP, ROI region QP less than 0");
        break;
    default:
        break;
    }
    /* Open the ROI region */
    bool ena_reg = true;
    if (y_size == 0
            || x_size == 0
            || roi_reg.len_x == 0
            || roi_reg.len_y == 0) {
        /* Close the ROI region */
        ena_reg = false;
    }
    esp_h264_mutex_lock(param->mutex, ESP_H264_MAX_DELAY);
    h264_hal_set_roi_reg(param->device, ena_reg, roi_reg.x, roi_reg.y, roi_reg.len_x, roi_reg.len_y, roi_reg.qp, roi_reg.reg_idx);
    esp_h264_mutex_unlock(param->mutex);
    return ESP_H264_ERR_OK;
}

static esp_h264_err_t get_roi_reg(esp_h264_enc_param_hw_handle_t handle, esp_h264_enc_roi_reg_t *roi_reg)
{
    esp_h264_param_t *param = __containerof(handle, esp_h264_param_t, hw_base);
    ESP_H264_RET_ON_FALSE(roi_reg->reg_idx < ESP_H264_ROI_SUP_NUM, ESP_H264_ERR_ARG, TAG, "Invalid `reg_idx` parameter");
    h264_hal_get_roi_reg(param->device, &roi_reg->x, &roi_reg->y, &roi_reg->len_x, &roi_reg->len_y, &roi_reg->qp, roi_reg->reg_idx);
    return ESP_H264_ERR_OK;
}

static esp_h264_err_t cfg_mv(esp_h264_enc_param_hw_handle_t handle, esp_h264_enc_mv_cfg_t cfg)
{
    esp_h264_param_t *param = __containerof(handle, esp_h264_param_t, hw_base);
    if (cfg.mv_fmt >= ESP_H264_MVM_FMT_INVALID || cfg.mv_fmt < ESP_H264_MVM_FMT_ALL
            || cfg.mv_mode >= ESP_H264_MVM_MODE_INVALID || cfg.mv_mode < ESP_H264_MVM_MODE_DISABLE) {
        ESP_H264_LOGE(TAG, "Configure is vaild. mv_mode %d mv_fmt %d", cfg.mv_fmt, cfg.mv_mode);
        return ESP_H264_ERR_ARG;
    }
    esp_h264_mutex_lock(param->mutex, ESP_H264_MAX_DELAY);
    h264_hal_set_mv_mode(param->device, (int8_t)cfg.mv_mode, (uint8_t)cfg.mv_fmt);
    esp_h264_mutex_unlock(param->mutex);
    return ESP_H264_ERR_OK;
}

static esp_h264_err_t get_mv_cfg_info(esp_h264_enc_param_hw_handle_t handle, esp_h264_enc_mv_cfg_t *cfg)
{
    esp_h264_param_t *param = __containerof(handle, esp_h264_param_t, hw_base);
    int8_t mv_mode = 0;
    uint8_t mv_fmt = 0;
    h264_hal_get_mv_mode(param->device, &mv_mode, &mv_fmt);
    cfg->mv_mode = mv_mode;
    cfg->mv_fmt = mv_fmt;
    return ESP_H264_ERR_OK;
}

static esp_h264_err_t set_mv_pkt(esp_h264_enc_param_hw_handle_t handle, esp_h264_enc_mvm_pkt_t mv_pkt)
{
    esp_h264_param_t *param = __containerof(handle, esp_h264_param_t, hw_base);
    param->mvm_buf = (uint8_t *)mv_pkt.data;
    param->mvm_buf_len = mv_pkt.len;
    return ESP_H264_ERR_OK;
}

static esp_h264_err_t get_mv_data_len(esp_h264_enc_param_hw_handle_t handle, uint32_t *length)
{
    esp_h264_param_t *param = __containerof(handle, esp_h264_param_t, hw_base);
    *length = h264_hal_get_mvm_data_len(param->device);
    esp_h264_cache_check_and_invalidate(param->mvm_buf, param->mvm_buf_len);
    return ESP_H264_ERR_OK;
}

static int max_refame_buffer_size(int16_t mb_width)
{
    /** H264_DMA_MACRO_SIZE + H264_DMA_HALF_MACRO_SIZE : Y(16) + U(4) + V(4) */
    return H264_DMA_3_LINES * H264_DMA_MACRO_SIZE * (H264_DMA_MACRO_SIZE + H264_DMA_HALF_MACRO_SIZE) * mb_width + HW_DMA_BUF_ALIGN_SIZE;
}

static int max_db_buffer_size(int16_t mb_width, int16_t mb_height)
{
    int buffer_size = mb_width * (mb_height - 1) * H264_DMA_DB_12_LINES_ROW_LENGTH
                      + mb_width * (H264_DMA_DB_12_LINES_ROW_LENGTH + H264_DMA_DB_4_LINES_ROW_LENGTH)
                      + mb_width * (mb_height - 1) * H264_DMA_DB_4_LINES_ROW_LENGTH;
    return buffer_size + ((HW_DMA_BUF_ALIGN_SIZE + 1) << 1);
}

static void cfg_dsc(h264_dma_desc_t *dsc, uint8_t en_2d, uint8_t mode, uint16_t vb, uint16_t hb, uint8_t eof, uint8_t owner, uint16_t va, uint16_t ha, uint8_t *buf, h264_dma_desc_t *next_dsc)
{
    dsc->dma_2d_en = en_2d;
    dsc->mode = mode;
    dsc->vb = vb;
    dsc->hb = hb;
    dsc->eof = eof;
    dsc->owner = owner;
    dsc->va = va;
    dsc->ha = ha;
    dsc->buf = buf;
    dsc->next_desc = next_dsc;
    esp_h264_cache_check_and_writeback((uint8_t *)dsc, sizeof(h264_dma_desc_t));
}

esp_h264_err_t esp_h264_enc_hw_del_param(esp_h264_enc_param_hw_handle_t handle)
{
    if (handle) {
        esp_h264_param_t *param = __containerof(handle, esp_h264_param_t, hw_base);
        if (param->nal_buf) {
            esp_h264_free(param->nal_buf);
        }
        if (param->ref) {
            esp_h264_free(param->ref);
        }
        if (param->db) {
            esp_h264_free(param->db);
        }
        if (param->dsc_ref) {
            esp_h264_free(param->dsc_ref);
        }
        for (size_t i = 0; i < 4; i++) {
            if (param->dsc_db[i]) {
                esp_h264_free(param->dsc_db[i]);
            }
        }
        if (param->dsc_mvm) {
            esp_h264_free(param->dsc_mvm);
        }
        esp_h264_enc_hw_rc_del(param->rc_hd);
        if (param->mutex) {
            esp_h264_mutex_delete(param->mutex);
        }
        esp_h264_free(param);
    }
    return ESP_H264_ERR_OK;
}

esp_h264_err_t esp_h264_enc_hw_new_param(esp_h264_enc_hw_param_cfg_t *cfg, esp_h264_enc_param_hw_handle_t *out_handle)
{
    /* Parameter check */
    ESP_H264_RET_ON_FALSE(cfg && out_handle, ESP_H264_ERR_ARG, TAG, "Invalid parameter");

    *out_handle = NULL;
    esp_h264_err_t ret = ESP_H264_ERR_OK;
    uint32_t actual_size;
    /** Create a new parameter handle */
    esp_h264_param_t *param = (esp_h264_param_t *)esp_h264_calloc_prefer(1, sizeof(esp_h264_param_t), &actual_size, ESP_H264_MEM_SPIRAM, ESP_H264_MEM_INTERNAL);
    ESP_H264_RET_ON_FALSE(param, ESP_H264_ERR_ARG, TAG, "No memory for handle");

    /* Parameter initalization */
    param->device = cfg->device;
    param->width = cfg->width;
    param->height = cfg->height;
    param->qp_init = (cfg->qp_min + cfg->qp_max) >> 1;
    param->fps = cfg->fps;
    param->bitrate = cfg->bitrate;
    h264_hal_set_qp(param->device, param->qp_init);
    h264_hal_get_mbres(param->device, &param->mb_width, &param->mb_height);

    /** Create RC handle */
    if (cfg->qp_min < cfg->qp_max) {
        param->rc_hd = esp_h264_enc_hw_rc_new(cfg->qp_max, cfg->qp_min, param->bitrate, param->fps, param->mb_width, param->mb_height);
        ESP_H264_GOTO_ON_FALSE(param->rc_hd, ret, __exit__, TAG, "No memory for RC");
    }
    /** Disable MV */
    h264_hal_set_mv_mode(param->device, (int8_t)ESP_H264_MVM_MODE_DISABLE, 0);

    /** SPS + PPS */
    param->nal_buf_len = SPS_PPS_BUF_SIZE;
    param->nal_buf = (uint8_t *)esp_h264_calloc_prefer(1, param->nal_buf_len, &actual_size, ESP_H264_MEM_INTERNAL, ESP_H264_MEM_SPIRAM);
    ESP_H264_GOTO_ON_FALSE(param->nal_buf, ESP_H264_ERR_MEM, __exit__, TAG, "No memory for NAL");
    param->nal_bit_len = esp_h264_enc_set_sps(param->nal_buf, param->nal_buf_len, param->height, param->width, param->fps);
    param->nal_bit_len += esp_h264_enc_set_pps(param->nal_buf + (param->nal_bit_len >> 3), param->nal_buf_len - (param->nal_bit_len >> 3), param->qp_init, true);

    /** Allocated reference frame and DB memory */
    param->ref = (uint8_t *)esp_h264_aligned_calloc(16, 1, max_refame_buffer_size(param->mb_width), &actual_size, ESP_H264_MEM_INTERNAL);
    ESP_H264_GOTO_ON_FALSE(param->ref, ESP_H264_ERR_MEM, __exit__, TAG, "No memory for reference frame");
    param->db = (uint8_t *)esp_h264_calloc_prefer(1, max_db_buffer_size(param->mb_width, param->mb_height), &actual_size, ESP_H264_MEM_INTERNAL, ESP_H264_MEM_SPIRAM);
    ESP_H264_GOTO_ON_FALSE(param->db, ESP_H264_ERR_MEM, __exit__, TAG, "No memory for data");

    /** Allocated descriptor memory*/
    param->dsc_ref = (h264_dma_desc_t *)esp_h264_aligned_calloc(16, 1, sizeof(h264_dma_desc_t), &actual_size, ESP_H264_MEM_INTERNAL);
    ESP_H264_GOTO_ON_FALSE(param->dsc_ref, ESP_H264_ERR_MEM, __exit__, TAG, "No memory for reference descriptor");
    for (size_t i = 0; i < 4; i++) {
        param->dsc_db[i] = (h264_dma_desc_t *)esp_h264_aligned_calloc(16, 1, sizeof(h264_dma_desc_t), &actual_size, ESP_H264_MEM_INTERNAL);
        ESP_H264_GOTO_ON_FALSE(param->dsc_db[i], ESP_H264_ERR_MEM, __exit__, TAG, "No memory for DB descriptor");
    }
    param->dsc_mvm = (h264_dma_desc_t *)esp_h264_aligned_calloc(16, 1, sizeof(h264_dma_desc_t), &actual_size, ESP_H264_MEM_INTERNAL);
    ESP_H264_GOTO_ON_FALSE(param->dsc_mvm, ESP_H264_ERR_MEM, __exit__, TAG, "No memory for MVM descriptor");

    /** Create MUTEX */
    param->mutex = xSemaphoreCreateMutex();
    ESP_H264_GOTO_ON_FALSE(param->mutex, ESP_H264_ERR_MEM, __exit__, TAG, "No memory for mutex semaphore");

    /** Encoder handle configure */
    param->hw_base.base.get_res = get_res;
    param->hw_base.base.set_fps = set_fps;
    param->hw_base.base.get_fps = get_fps;
    param->hw_base.base.set_gop = set_gop;
    param->hw_base.base.get_gop = get_gop;
    param->hw_base.base.set_bitrate = set_bitrate;
    param->hw_base.base.get_bitrate = get_bitrate;
    param->hw_base.cfg_mv = cfg_mv;
    param->hw_base.get_mv_cfg_info = get_mv_cfg_info;
    param->hw_base.set_mv_pkt = set_mv_pkt;
    param->hw_base.get_mv_data_len = get_mv_data_len;
    param->hw_base.cfg_roi = cfg_roi;
    param->hw_base.get_roi_cfg_info = get_roi_cfg_info;
    param->hw_base.set_roi_reg = set_roi_reg;
    param->hw_base.get_roi_reg = get_roi_reg;
    *out_handle = &param->hw_base;
    return ret;
__exit__:
    /** Delete the handle */
    esp_h264_enc_hw_del_param(&param->hw_base);
    return ret;
}

esp_h264_err_t esp_h264_enc_hw_get_mutex(esp_h264_enc_param_hw_handle_t handle, esp_h264_mutex_t *out_mutex)
{
    esp_h264_param_t *param = __containerof(handle, esp_h264_param_t, hw_base);
    *out_mutex = param->mutex;
    return ESP_H264_ERR_OK;
}

esp_h264_err_t esp_h264_enc_hw_get_qp_init(esp_h264_enc_param_hw_handle_t handle, uint8_t *out_qp_init)
{
    esp_h264_param_t *param = __containerof(handle, esp_h264_param_t, hw_base);
    *out_qp_init = param->qp_init;
    return ESP_H264_ERR_OK;
}

esp_h264_err_t esp_h264_enc_hw_get_nal(esp_h264_enc_param_hw_handle_t handle, uint8_t *out_nal_buf, uint16_t *out_nal_bit_len)
{
    esp_h264_param_t *param = __containerof(handle, esp_h264_param_t, hw_base);
    memcpy(out_nal_buf, param->nal_buf, param->nal_bit_len >> 3);
    *out_nal_bit_len = param->nal_bit_len;
    return ESP_H264_ERR_OK;
}

esp_h264_err_t esp_h264_enc_hw_cfg_dma_db_ref(esp_h264_enc_param_hw_handle_t handle, h264_dma_hal_context_t *dma2d_hal)
{
    esp_h264_param_t *param = __containerof(handle, esp_h264_param_t, hw_base);
    uint32_t buff_addr = (uint32_t)ALIGN_UP((uint32_t)param->ref, 8);
    cfg_dsc(param->dsc_ref, H264_DMA_2D_ENABLE, H264_DMA_MODE1, H264_DMA_3_LINES, H264_DMA_MACRO_SIZE * H264_DMA_MACRO_SIZE, H264_DMA_EOF_END,
            H264_DMA_OWNER_H264, H264_DMA_3_LINES, H264_DMA_MACRO_SIZE * H264_DMA_MACRO_SIZE * param->mb_width, (uint8_t *)buff_addr, param->dsc_ref);
    h264_dma_hal_cfg_ref_dsc(dma2d_hal, (uint32_t)param->dsc_ref, buff_addr, param->mb_width);
    uint32_t size = H264_DMA_DB_12_LINES_ROW_LENGTH * param->mb_width * (param->mb_height - 1)
                    + (H264_DMA_DB_12_LINES_ROW_LENGTH + H264_DMA_DB_4_LINES_ROW_LENGTH) * param->mb_width;
    buff_addr = (uint32_t)ALIGN_UP((uint32_t)param->db, 8);
    cfg_dsc(param->dsc_db[0], H264_DMA_2D_DISABLE, H264_DMA_MODE0, size & H264_DMA_MAX_SIZE, size & H264_DMA_MAX_SIZE, H264_DMA_EOF_END, H264_DMA_OWNER_H264,
            (size >> H264_DMA_SIZE_BIT), (size >> H264_DMA_SIZE_BIT), (uint8_t *)buff_addr, param->dsc_db[0]);
    cfg_dsc(param->dsc_db[1], H264_DMA_2D_DISABLE, H264_DMA_MODE0, size & H264_DMA_MAX_SIZE, size & H264_DMA_MAX_SIZE, H264_DMA_EOF_END, H264_DMA_OWNER_H264,
            (size >> H264_DMA_SIZE_BIT), (size >> H264_DMA_SIZE_BIT), (uint8_t *)buff_addr, param->dsc_db[1]);
    buff_addr = (uint32_t)ALIGN_UP((uint32_t)buff_addr + size, 8);
    size = H264_DMA_DB_4_LINES_ROW_LENGTH * param->mb_width * (param->mb_height - 1);
    cfg_dsc(param->dsc_db[2], H264_DMA_2D_DISABLE, H264_DMA_MODE0, size & H264_DMA_MAX_SIZE, size & H264_DMA_MAX_SIZE, H264_DMA_EOF_END, H264_DMA_OWNER_H264,
            (size >> H264_DMA_SIZE_BIT), (size >> H264_DMA_SIZE_BIT), (uint8_t *)buff_addr, param->dsc_db[2]);
    cfg_dsc(param->dsc_db[3], H264_DMA_2D_DISABLE, H264_DMA_MODE0, size & H264_DMA_MAX_SIZE, size & H264_DMA_MAX_SIZE, H264_DMA_EOF_END, H264_DMA_OWNER_H264,
            (size >> H264_DMA_SIZE_BIT), (size >> H264_DMA_SIZE_BIT), (uint8_t *)buff_addr, param->dsc_db[3]);
    h264_dma_hal_cfg_db12_4_dsc(dma2d_hal, (uint32_t *)param->dsc_db);
    return ESP_H264_ERR_OK;
}

esp_h264_err_t esp_h264_enc_hw_cfg_dma_yuv_bs(esp_h264_enc_param_hw_handle_t handle, h264_dma_hal_context_t *dma2d_hal, h264_dma_desc_t *dsc_yuv, uint8_t *buf_yuv, h264_dma_desc_t *dsc_bs, uint8_t *buf_bs, uint32_t buf_bs_len)
{
    esp_h264_param_t *param = __containerof(handle, esp_h264_param_t, hw_base);
    cfg_dsc(dsc_yuv, H264_DMA_2D_ENABLE, H264_DMA_MODE1, H264_DMA_MACRO_SIZE, H264_DMA_MACRO_SIZE * H264_DMA_4_LINES, H264_DMA_EOF_CONTINUE, H264_DMA_OWNER_H264,
            param->height, param->width, buf_yuv, NULL);
    h264_dma_hal_cfg_yuv_dsc(dma2d_hal, (uint32_t)dsc_yuv);
    cfg_dsc(dsc_bs, H264_DMA_2D_DISABLE, H264_DMA_MODE0, buf_bs_len & H264_DMA_MAX_SIZE, 0, H264_DMA_EOF_END, H264_DMA_OWNER_H264, (buf_bs_len >> H264_DMA_SIZE_BIT),
            0, buf_bs, NULL);
    h264_dma_hal_cfg_bs_dsc(dma2d_hal, (uint32_t)dsc_bs);
    return ESP_H264_ERR_OK;
}

esp_h264_err_t esp_h264_enc_hw_cfg_dma_dbtmp(esp_h264_enc_param_hw_handle_t handle, h264_dma_hal_context_t *dma2d_hal, h264_dma_desc_t *dsc[2], uint8_t *buf)
{
    esp_h264_param_t *param = __containerof(handle, esp_h264_param_t, hw_base);
    uint32_t size = H264_DMA_DB_4_LINES_ROW_LENGTH * param->mb_width;
    cfg_dsc(dsc[0], H264_DMA_2D_DISABLE, H264_DMA_MODE0, size & H264_DMA_MAX_SIZE, size & H264_DMA_MAX_SIZE, H264_DMA_EOF_END, H264_DMA_OWNER_H264,
            (size >> H264_DMA_SIZE_BIT), (size >> H264_DMA_SIZE_BIT), buf, dsc[0]);
    cfg_dsc(dsc[1], H264_DMA_2D_DISABLE, H264_DMA_MODE0, size & H264_DMA_MAX_SIZE, size & H264_DMA_MAX_SIZE, H264_DMA_EOF_END, H264_DMA_OWNER_H264,
            (size >> H264_DMA_SIZE_BIT), (size >> H264_DMA_SIZE_BIT), buf, dsc[1]);
    h264_dma_hal_cfg_dbtmp_dsc(dma2d_hal, (uint32_t *)dsc);
    return ESP_H264_ERR_OK;
}

esp_h264_err_t esp_h264_enc_hw_cfg_dma_mvm(esp_h264_enc_param_hw_handle_t handle, h264_dma_hal_context_t *dma2d_hal)
{
    esp_h264_param_t *param = __containerof(handle, esp_h264_param_t, hw_base);
    int8_t mv_mode = 0;
    uint8_t mv_fmt = 0;
    h264_hal_get_mv_mode(param->device, &mv_mode, &mv_fmt);
    if (mv_mode == ESP_H264_MVM_MODE_DISABLE) {
        return ESP_H264_ERR_OK;
    }
    if (param->mvm_buf == NULL) {
        return ESP_H264_ERR_FAIL;
    }
    cfg_dsc(param->dsc_mvm, H264_DMA_2D_DISABLE, H264_DMA_MODE0, param->mvm_buf_len & H264_DMA_MAX_SIZE, 0, H264_DMA_EOF_END, H264_DMA_OWNER_H264,
            (param->mvm_buf_len >> H264_DMA_SIZE_BIT), 0, param->mvm_buf, NULL);
    h264_dma_hal_cfg_mvm_dsc(dma2d_hal, (uint32_t)param->dsc_mvm);
    return ESP_H264_ERR_OK;
}

esp_h264_err_t esp_h264_enc_hw_set_qp(esp_h264_enc_param_hw_handle_t handle, uint8_t qp)
{
    esp_h264_param_t *param = __containerof(handle, esp_h264_param_t, hw_base);
    h264_hal_set_qp(param->device, qp);
    return ESP_H264_ERR_OK;
}

esp_h264_err_t esp_h264_enc_hw_get_rc_hd(esp_h264_enc_param_hw_handle_t handle, esp_h264_rc_hd_t *out_rc_hd)
{
    esp_h264_param_t *param = __containerof(handle, esp_h264_param_t, hw_base);
    *out_rc_hd = param->rc_hd;
    return ESP_H264_ERR_OK;
}

esp_h264_err_t esp_h264_enc_hw_set_rc_rate_pred(esp_h264_enc_param_hw_handle_t handle, int32_t u, int32_t pred)
{
    esp_h264_param_t *param = __containerof(handle, esp_h264_param_t, hw_base);
    h264_hal_set_rc_rate_pred(param->device, u, pred);
    return ESP_H264_ERR_OK;
}

esp_h264_err_t esp_h264_enc_hw_get_mbres(esp_h264_enc_param_hw_handle_t handle, uint8_t *out_mb_width, uint8_t *out_mb_height)
{
    esp_h264_param_t *param = __containerof(handle, esp_h264_param_t, hw_base);
    h264_hal_get_mbres(param->device, out_mb_width, out_mb_height);
    return ESP_H264_ERR_OK;
}

esp_h264_err_t esp_h264_enc_hw_res_check(uint16_t width, uint16_t height)
{
    if ((width >= ESP_H264_MIN_WIDTH)
            && (width <= ESP_H264_MAX_WIDTH)
            && (height >= ESP_H264_MIN_HEIGHT)
            && (height <= ESP_H264_MAX_HEIGHT)) {
        return ESP_H264_ERR_OK;
    }
    return ESP_H264_ERR_FAIL;
}

/*  Dual stream mode, DB tmp  buffer can be resused. */
uint16_t esp_h264_enc_hw_max_db_tmp_buffer_size(uint16_t width)
{
    uint16_t mb_xsize = (width + 15) >> 4;
    return mb_xsize * H264_DMA_DB_4_LINES_ROW_LENGTH + HW_DMA_BUF_ALIGN_SIZE + ESP_H264_REDUNDANT_BYTE;
}

uint8_t *esp_h264_enc_hw_slice_header_align8(uint8_t *bs, int32_t bit_len, h264_hal_context_t *h264_hal)
{
    int32_t blen = bit_len >> 3;
    int32_t unaligned_blen = (blen & HW_DMA_BUF_ALIGN_SIZE);
    uint32_t header[3];
    uint8_t *src = bs + (blen & (~HW_DMA_BUF_ALIGN_SIZE));
    uint8_t *dst = (uint8_t *)(&header[0]);
    for (int32_t i = 0; i < unaligned_blen; i++) {
        dst[7 - i] = src[i];
    }
    header[2] = bs[((bit_len + HW_DMA_BUF_ALIGN_SIZE)  >> 3) - 1];
    int32_t slice_header_bits = (unaligned_blen << 3) + (bit_len & HW_DMA_BUF_ALIGN_SIZE);
    h264_hal_set_slice_header(h264_hal, header, slice_header_bits);
    return src;
}
