/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
// #include "esp_lcd_mipi_dsi.h"
#include "esp_lcd_panel_ops.h"
#include "esp_ldo_regulator.h"
#include "esp_cache.h"
#include "driver/i2c_master.h"
#include "driver/isp.h"
#include "esp_cam_ctlr_csi.h"
#include "esp_cam_ctlr.h"
// #include "example_dsi_init.h"
// #include "example_dsi_init_config.h"
#include "example_sensor_init.h"
#include "example_config.h"

static const char *TAG = "cam_dsi";

bool cam_buff_ready = false;

static bool s_camera_get_new_vb(esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, void *user_data);
static bool s_camera_get_finished_trans(esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, void *user_data);

#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
// #include "esp_lcd_panel_ops.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "driver/ppa.h"

#include "esp_lcd_ili9341.h"
#include "esp_check.h"

// Using SPI2 in the example
#define LCD_HOST  SPI2_HOST

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// Please update the following configuration according to your LCD spec //////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define EXAMPLE_LCD_PIXEL_CLOCK_HZ     (20 * 1000 * 1000)
#define EXAMPLE_LCD_BK_LIGHT_ON_LEVEL  1
#define EXAMPLE_LCD_BK_LIGHT_OFF_LEVEL !EXAMPLE_LCD_BK_LIGHT_ON_LEVEL

#define EXAMPLE_PIN_NUM_SCLK           4       // UEXT: 9
#define EXAMPLE_PIN_NUM_MOSI           53      // UEXT: 8
#define EXAMPLE_PIN_NUM_MISO           20 
#define EXAMPLE_PIN_NUM_LCD_DC         54      // UEXT: 7
#define EXAMPLE_PIN_NUM_LCD_RST        21
#define EXAMPLE_PIN_NUM_LCD_CS         5       // UEXT: 10
#define EXAMPLE_PIN_NUM_BK_LIGHT       22
#define EXAMPLE_PIN_NUM_TOUCH_CS       23

// The pixel number in horizontal and vertical
#define EXAMPLE_LCD_H_RES              320
#define EXAMPLE_LCD_V_RES              240
// Bit number used to represent command and parameter
#define EXAMPLE_LCD_CMD_BITS           8
#define EXAMPLE_LCD_PARAM_BITS         8

esp_err_t example_lcd_init(esp_lcd_panel_handle_t *lcd_handle, void **lcd_buffer, size_t *lcd_buffer_size) {
    ESP_LOGI(TAG, "LCD buffer allocation");
    *lcd_buffer_size = EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES * 3;
    *lcd_buffer = heap_caps_malloc(*lcd_buffer_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA);
    ESP_RETURN_ON_FALSE(*lcd_buffer, ESP_ERR_NO_MEM, TAG, "FAIL [%d]", *lcd_buffer_size);

    ESP_LOGI(TAG, "LCD turn off backlight");
    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << EXAMPLE_PIN_NUM_BK_LIGHT
    };
    ESP_RETURN_ON_ERROR(gpio_config(&bk_gpio_config), TAG, "FAIL");

    ESP_LOGI(TAG, "LCD Initialize SPI bus");
    spi_bus_config_t buscfg = {
        .sclk_io_num = EXAMPLE_PIN_NUM_SCLK,
        .mosi_io_num = EXAMPLE_PIN_NUM_MOSI,
        .miso_io_num = EXAMPLE_PIN_NUM_MISO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        // .max_transfer_sz = EXAMPLE_LCD_V_RES * 80 * sizeof(uint16_t),
    };
    ESP_RETURN_ON_ERROR(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO), TAG, "FAIL");

    ESP_LOGI(TAG, "LCD install SPI IO");
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = EXAMPLE_PIN_NUM_LCD_DC,
        .cs_gpio_num = EXAMPLE_PIN_NUM_LCD_CS,
        .pclk_hz = EXAMPLE_LCD_PIXEL_CLOCK_HZ,
        .lcd_cmd_bits = EXAMPLE_LCD_CMD_BITS,
        .lcd_param_bits = EXAMPLE_LCD_PARAM_BITS,
        .spi_mode = 0,
        .trans_queue_depth = 10,
    };
    // Attach the LCD to the SPI bus
    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST, &io_config, &io_handle), TAG, "FAIL");

    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = EXAMPLE_PIN_NUM_LCD_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_BGR,
        .bits_per_pixel = 16,
    };
    ESP_LOGI(TAG, "LCD Install ILI9341 driver");
    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_ili9341(io_handle, &panel_config, lcd_handle), TAG, "FAIL");

    ESP_RETURN_ON_ERROR(esp_lcd_panel_reset(*lcd_handle), TAG, "Reset FAIL");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_init(*lcd_handle), TAG, "Init FAIL");

    ESP_RETURN_ON_ERROR(esp_lcd_panel_swap_xy(*lcd_handle, true), TAG, "Swap FAIL");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_mirror(*lcd_handle, true, false), TAG, "Mirror FAIL");

    // user can flush pre-defined pattern to the screen before we turn on the screen or backlight
    ESP_RETURN_ON_ERROR(esp_lcd_panel_disp_on_off(*lcd_handle, true), TAG, "On/Off FAIL");

    ESP_LOGI(TAG, "LCD turn on backlight");
    gpio_set_level(EXAMPLE_PIN_NUM_BK_LIGHT, EXAMPLE_LCD_BK_LIGHT_ON_LEVEL);

    return ESP_OK;
}

esp_err_t example_ppa_init(ppa_client_handle_t *ppa_srm_handle) {
    //---------------SRM------------------//
    ppa_client_config_t ppa_srm_config = {
        .oper_type = PPA_OPERATION_SRM,
        .max_pending_trans_num = 1,
    };
    ESP_RETURN_ON_ERROR(ppa_register_client(&ppa_srm_config, ppa_srm_handle), TAG, "PPA SRM Init FAIL");
    return ESP_OK;
}

esp_err_t cam_to_lcd_transform(void *in_buf, void *out_buf, size_t buf_size, ppa_client_handle_t ppa_srm_handle) {
    ppa_srm_oper_config_t srm_config = {
        .in.buffer = in_buf,
        .in.pic_w = CONFIG_EXAMPLE_MIPI_CSI_DISP_HRES,
        .in.pic_h = CONFIG_EXAMPLE_MIPI_CSI_DISP_VRES,
        .in.block_w = EXAMPLE_LCD_H_RES,
        .in.block_h = EXAMPLE_LCD_V_RES,
        .in.block_offset_x = (CONFIG_EXAMPLE_MIPI_CSI_DISP_HRES - EXAMPLE_LCD_H_RES) / 2,
        .in.block_offset_y = (CONFIG_EXAMPLE_MIPI_CSI_DISP_VRES - EXAMPLE_LCD_V_RES) / 2,
        .in.srm_cm = PPA_SRM_COLOR_MODE_RGB565,
        .out.buffer = out_buf,
        .out.buffer_size = buf_size,
        .out.pic_w = EXAMPLE_LCD_H_RES,
        .out.pic_h = EXAMPLE_LCD_V_RES,
        .out.block_offset_x = 0,
        .out.block_offset_y = 0,
        .out.srm_cm = PPA_SRM_COLOR_MODE_RGB565,
        .rotation_angle = PPA_SRM_ROTATION_ANGLE_0,
        .scale_x = 1,
        .scale_y = 1,
        .rgb_swap = 0,
        .byte_swap = 1,
        .mode = PPA_TRANS_MODE_BLOCKING,
    };

    if (false) {
        srm_config.in.block_w = CONFIG_EXAMPLE_MIPI_CSI_DISP_HRES,
        srm_config.in.block_h = CONFIG_EXAMPLE_MIPI_CSI_DISP_VRES,
        srm_config.in.block_offset_x = 0;
        srm_config.in.block_offset_y = 0;
        srm_config.scale_x = (float)(1.0 * srm_config.out.pic_w / srm_config.in.pic_w);
        srm_config.scale_y = (float)(1.0 * srm_config.out.pic_h / srm_config.in.pic_h);
    }

    ESP_RETURN_ON_ERROR(ppa_do_scale_rotate_mirror(ppa_srm_handle, &srm_config), TAG, "PPA SRM Failed");
    return ESP_OK;
}

void app_main(void)
{
    //mipi ldo
    esp_ldo_channel_handle_t ldo_mipi_phy = NULL;
    esp_ldo_channel_config_t ldo_mipi_phy_config = {
        .chan_id = CONFIG_EXAMPLE_USED_LDO_CHAN_ID,
        .voltage_mv = CONFIG_EXAMPLE_USED_LDO_VOLTAGE_MV,
    };
    ESP_RETURN_VOID_ON_ERROR(esp_ldo_acquire_channel(&ldo_mipi_phy_config, &ldo_mipi_phy), TAG, "MIPI LDO init FAIL");
    ESP_LOGI(TAG, "MIPI LDO init OK");
    
    ppa_client_handle_t ppa_handle;
    ESP_RETURN_VOID_ON_ERROR(example_ppa_init(&ppa_handle), TAG, "PPA init FAIL");
    ESP_LOGI(TAG, "PPA SRM Init OK");

    /**
     * @background
     * Sensor use RAW8
     * ISP convert to RGB565
     */
  
    esp_lcd_panel_handle_t lcd_handle;
    void *lcd_buffer = NULL;
    size_t lcd_buffer_size;
    ESP_RETURN_VOID_ON_ERROR(example_lcd_init(&lcd_handle, &lcd_buffer, &lcd_buffer_size), TAG, "LCD init FAIL");
    ESP_LOGI(TAG, "LCD init OK");

    //--------Camera Sensor and SCCB Init-----------//
    i2c_master_bus_handle_t i2c_bus_handle = NULL;
    example_sensor_init(I2C_NUM_0, &i2c_bus_handle);
    ESP_RETURN_VOID_ON_FALSE(i2c_bus_handle, TAG, "Camera sensor init  [Fail]");

    //---------------CSI Init------------------//
    esp_cam_ctlr_csi_config_t csi_config = {
        .ctlr_id = 0,
        .h_res = CONFIG_EXAMPLE_MIPI_CSI_DISP_HRES,
        .v_res = CONFIG_EXAMPLE_MIPI_CSI_DISP_VRES,
        .lane_bit_rate_mbps = EXAMPLE_MIPI_CSI_LANE_BITRATE_MBPS,
        .input_data_color_type = CAM_CTLR_COLOR_RAW8,
        .output_data_color_type = CAM_CTLR_COLOR_RAW8,
        .data_lane_num = 2,
        .byte_swap_en = false,
        .queue_items = 1,
    };
    esp_cam_ctlr_handle_t cam_handle = NULL;
    ESP_RETURN_VOID_ON_ERROR(esp_cam_new_csi_ctlr(&csi_config, &cam_handle), TAG, "CSI init FAIL");

    void *cam_buffer_internal = NULL;
    ESP_RETURN_VOID_ON_ERROR(esp_cam_ctlr_get_frame_buffer(cam_handle, 1, (const void **)&cam_buffer_internal), TAG, "Camera internal buff get FAIL");
    ESP_LOGI(TAG, "Camera internal buff get OK");

    size_t cam_buffer_size = 0;
    ESP_RETURN_VOID_ON_ERROR(esp_cam_ctlr_get_frame_buffer_len(cam_handle, &cam_buffer_size), TAG,"Camera get buffer len FAIL");

    void *cam_buffer = heap_caps_malloc(cam_buffer_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA);
    ESP_RETURN_VOID_ON_FALSE(cam_buffer, TAG, "Camera buffer allocation  [Fail %d]", cam_buffer_size);
    ESP_LOGI(TAG, "Buffer allocation  [OK %d]", cam_buffer_size);

    esp_cam_ctlr_trans_t new_trans = {
        .buffer = cam_buffer,
        .buflen = cam_buffer_size,
    };

    esp_cam_ctlr_evt_cbs_t cbs = {
        .on_get_new_trans = s_camera_get_new_vb,
        .on_trans_finished = s_camera_get_finished_trans,
    };
    ESP_RETURN_VOID_ON_ERROR(esp_cam_ctlr_register_event_callbacks(cam_handle, &cbs, &new_trans), TAG, "Camera register callback FAIL");

    ESP_RETURN_VOID_ON_ERROR(esp_cam_ctlr_enable(cam_handle), TAG, "Camera enable FAIL");

    //---------------ISP Init------------------//
    isp_proc_handle_t isp_proc = NULL;
    esp_isp_processor_cfg_t isp_config = {
        .clk_hz = 80 * 1000 * 1000,
        .input_data_source = ISP_INPUT_DATA_SOURCE_CSI,
        .input_data_color_type = ISP_COLOR_RAW8,
        .output_data_color_type = ISP_COLOR_RGB565,
        .bayer_order = COLOR_RAW_ELEMENT_ORDER_BGGR,
        .has_line_start_packet = false,
        .has_line_end_packet = false,
        .h_res = CONFIG_EXAMPLE_MIPI_CSI_DISP_HRES,
        .v_res = CONFIG_EXAMPLE_MIPI_CSI_DISP_VRES,
    };
    ESP_RETURN_VOID_ON_ERROR(esp_isp_new_processor(&isp_config, &isp_proc), TAG, "ISP init FAIL");
    ESP_RETURN_VOID_ON_ERROR(esp_isp_enable(isp_proc), TAG, "ISP enable FAIL");
    
    // Clear LCD buffer
    memset(lcd_buffer, 0x00, lcd_buffer_size);
    esp_lcd_panel_draw_bitmap(lcd_handle, 0, 0, EXAMPLE_LCD_H_RES, EXAMPLE_LCD_V_RES, lcd_buffer);

    if (esp_cam_ctlr_start(cam_handle) != ESP_OK) {
        ESP_LOGE(TAG, "Driver start fail");
        return;
    }

    esp_cam_ctlr_receive(cam_handle, &new_trans, 1000);
    
    ESP_LOGI(TAG, "READY");
    while (1) {
        // ESP_ERROR_CHECK(esp_cam_ctlr_receive(cam_handle, &new_trans, ESP_CAM_CTLR_MAX_DELAY));
        if (cam_buff_ready) {
            ESP_RETURN_VOID_ON_ERROR(cam_to_lcd_transform(cam_buffer, lcd_buffer, lcd_buffer_size, ppa_handle), TAG, "FAIL");
            esp_lcd_panel_draw_bitmap(lcd_handle, 0, 0, EXAMPLE_LCD_H_RES, EXAMPLE_LCD_V_RES, lcd_buffer);
            cam_buff_ready = false;
        }
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }
}

static bool s_camera_get_new_vb(esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, void *user_data)
{
    esp_cam_ctlr_trans_t new_trans = *(esp_cam_ctlr_trans_t *)user_data;
    trans->buffer = new_trans.buffer;
    trans->buflen = new_trans.buflen;

    cam_buff_ready = false;

    return false;
}

static bool s_camera_get_finished_trans(esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, void *user_data)
{
    cam_buff_ready = true;
    return false;
}
