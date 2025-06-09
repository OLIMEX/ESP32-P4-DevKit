/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/lock.h>
#include <sys/param.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_timer.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_mipi_dsi.h"
#include "esp_ldo_regulator.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "lvgl.h"
#include "esp_lcd_ili9881c.h"
#include "esp_lcd_ek79007.h"

#include "esp_lcd_st7701.h"
#include "esp_io_expander_pca9536.h"

static const char *TAG = "example";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// Please update the following configuration according to your LCD Spec //////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if CONFIG_EXAMPLE_LCD_USE_ILI9881C
// Refresh Rate = 80000000/(40+140+40+800)/(4+16+16+1280) = 60Hz
#define EXAMPLE_MIPI_DSI_DPI_CLK_MHZ  80
#define EXAMPLE_MIPI_DSI_LCD_H_RES    800
#define EXAMPLE_MIPI_DSI_LCD_V_RES    1280
#define EXAMPLE_MIPI_DSI_LCD_HSYNC    40
#define EXAMPLE_MIPI_DSI_LCD_HBP      140
#define EXAMPLE_MIPI_DSI_LCD_HFP      40
#define EXAMPLE_MIPI_DSI_LCD_VSYNC    4
#define EXAMPLE_MIPI_DSI_LCD_VBP      16
#define EXAMPLE_MIPI_DSI_LCD_VFP      16
#elif CONFIG_EXAMPLE_LCD_USE_EK79007
// Refresh Rate = 48000000/(10+120+120+1024)/(1+20+10+600) = 60Hz
#define EXAMPLE_MIPI_DSI_DPI_CLK_MHZ  48
#define EXAMPLE_MIPI_DSI_LCD_H_RES    1024
#define EXAMPLE_MIPI_DSI_LCD_V_RES    600
#define EXAMPLE_MIPI_DSI_LCD_HSYNC    10
#define EXAMPLE_MIPI_DSI_LCD_HBP      120
#define EXAMPLE_MIPI_DSI_LCD_HFP      120
#define EXAMPLE_MIPI_DSI_LCD_VSYNC    1
#define EXAMPLE_MIPI_DSI_LCD_VBP      20
#define EXAMPLE_MIPI_DSI_LCD_VFP      10
#elif CONFIG_EXAMPLE_LCD_USE_ST7701
// Refresh Rate = 25000000/(10+4+20+480)/(8+4+14+640) = 73Hz
#define EXAMPLE_MIPI_DSI_DPI_CLK_MHZ  25
#define EXAMPLE_MIPI_DSI_LCD_H_RES    480
#define EXAMPLE_MIPI_DSI_LCD_V_RES    640
#define EXAMPLE_MIPI_DSI_LCD_HSYNC    4
#define EXAMPLE_MIPI_DSI_LCD_HBP      20
#define EXAMPLE_MIPI_DSI_LCD_HFP      10
#define EXAMPLE_MIPI_DSI_LCD_VSYNC    4
#define EXAMPLE_MIPI_DSI_LCD_VBP      14
#define EXAMPLE_MIPI_DSI_LCD_VFP      8
#endif

#if CONFIG_EXAMPLE_LCD_USE_ST7701
#define EXAMPLE_MIPI_DSI_LANE_NUM          1    // 1 data lanes
#else
#define EXAMPLE_MIPI_DSI_LANE_NUM          2    // 2 data lanes
#endif

#define EXAMPLE_MIPI_DSI_LANE_BITRATE_MBPS 1000 // 1Gbps
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// Please update the following configuration according to your Board Design //////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// The "VDD_MIPI_DPHY" should be supplied with 2.5V, it can source from the internal LDO regulator or from external LDO chip
#define EXAMPLE_MIPI_DSI_PHY_PWR_LDO_CHAN       3  // LDO_VO3 is connected to VDD_MIPI_DPHY
#define EXAMPLE_MIPI_DSI_PHY_PWR_LDO_VOLTAGE_MV 2500

#define EXAMPLE_LCD_BK_LIGHT_ON_LEVEL           1
#define EXAMPLE_LCD_BK_LIGHT_OFF_LEVEL          !EXAMPLE_LCD_BK_LIGHT_ON_LEVEL
#define EXAMPLE_PIN_NUM_BK_LIGHT                -1
#define EXAMPLE_PIN_NUM_LCD_RST                 -1

#if CONFIG_EXAMPLE_MONITOR_REFRESH_BY_GPIO
#define EXAMPLE_PIN_NUM_REFRESH_MONITOR         20  // Monitor the Refresh Rate by toggling the GPIO
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// Please update the following configuration according to your Application ///////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define EXAMPLE_LVGL_DRAW_BUF_LINES    (EXAMPLE_MIPI_DSI_LCD_V_RES / 10) // number of display lines in each draw buffer
#define EXAMPLE_LVGL_TICK_PERIOD_MS    2
#define EXAMPLE_LVGL_TASK_STACK_SIZE   (4 * 1024)
#define EXAMPLE_LVGL_TASK_PRIORITY     2
#define EXAMPLE_LVGL_TASK_MAX_DELAY_MS 500
#define EXAMPLE_LVGL_TASK_MIN_DELAY_MS 1000 / CONFIG_FREERTOS_HZ

// LVGL library is not thread-safe, this example will call LVGL APIs from different tasks, so use a mutex to protect it
static _lock_t lvgl_api_lock;

extern void example_lvgl_demo_ui(lv_display_t *disp);

static void example_lvgl_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    esp_lcd_panel_handle_t panel_handle = lv_display_get_user_data(disp);
    int offsetx1 = area->x1;
    int offsetx2 = area->x2;
    int offsety1 = area->y1;
    int offsety2 = area->y2;
    // pass the draw buffer to the driver
    esp_lcd_panel_draw_bitmap(panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, px_map);
}

static void example_increase_lvgl_tick(void *arg)
{
    /* Tell LVGL how many milliseconds has elapsed */
    lv_tick_inc(EXAMPLE_LVGL_TICK_PERIOD_MS);
}

static void example_lvgl_port_task(void *arg)
{
    ESP_LOGI(TAG, "Starting LVGL task");
    uint32_t time_till_next_ms = 0;
    while (1) {
        _lock_acquire(&lvgl_api_lock);
        time_till_next_ms = lv_timer_handler();
        _lock_release(&lvgl_api_lock);
        // in case of triggering a task watch dog time out
        time_till_next_ms = MAX(time_till_next_ms, EXAMPLE_LVGL_TASK_MIN_DELAY_MS);
        // in case of lvgl display not ready yet
        time_till_next_ms = MIN(time_till_next_ms, EXAMPLE_LVGL_TASK_MAX_DELAY_MS);
        usleep(1000 * time_till_next_ms);
    }
}

static bool example_notify_lvgl_flush_ready(esp_lcd_panel_handle_t panel, esp_lcd_dpi_panel_event_data_t *edata, void *user_ctx)
{
    lv_display_t *disp = (lv_display_t *)user_ctx;
    lv_display_flush_ready(disp);
    return false;
}

#if CONFIG_EXAMPLE_MONITOR_REFRESH_BY_GPIO
static bool example_monitor_refresh_rate(esp_lcd_panel_handle_t panel, esp_lcd_dpi_panel_event_data_t *edata, void *user_ctx)
{
    static int io_level = 0;
    // please note, the real refresh rate should be 2*frequency of this GPIO toggling
    gpio_set_level(EXAMPLE_PIN_NUM_REFRESH_MONITOR, io_level);
    io_level = !io_level;
    return false;
}
#endif

static void example_bsp_enable_dsi_phy_power(void)
{
#ifdef EXAMPLE_MIPI_DSI_PHY_PWR_LDO_CHAN
    // Turn on the power for MIPI DSI PHY, so it can go from "No Power" state to "Shutdown" state
    esp_ldo_channel_handle_t ldo_mipi_phy = NULL;
    esp_ldo_channel_config_t ldo_mipi_phy_config = {
        .chan_id = EXAMPLE_MIPI_DSI_PHY_PWR_LDO_CHAN,
        .voltage_mv = EXAMPLE_MIPI_DSI_PHY_PWR_LDO_VOLTAGE_MV,
    };
    ESP_ERROR_CHECK(esp_ldo_acquire_channel(&ldo_mipi_phy_config, &ldo_mipi_phy));
    ESP_LOGI(TAG, "MIPI DSI PHY Powered on");
#endif
}

static void example_bsp_init_lcd_backlight(void)
{
#if EXAMPLE_PIN_NUM_BK_LIGHT >= 0
    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << EXAMPLE_PIN_NUM_BK_LIGHT
    };
    ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));
#endif
}

static void example_bsp_set_lcd_backlight(uint32_t level)
{
#if EXAMPLE_PIN_NUM_BK_LIGHT >= 0
    gpio_set_level(EXAMPLE_PIN_NUM_BK_LIGHT, level);
#endif
}

#if CONFIG_EXAMPLE_MONITOR_REFRESH_BY_GPIO
static void example_bsp_init_refresh_monitor_io(void)
{
    gpio_config_t monitor_io_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << EXAMPLE_PIN_NUM_REFRESH_MONITOR,
    };
    ESP_ERROR_CHECK(gpio_config(&monitor_io_conf));
}
#endif

#if CONFIG_EXAMPLE_LCD_USE_ST7701
static const st7701_lcd_init_cmd_t lcd_init_cmds[] = {
    // Command2 BK3 Selection: Enable the BK function of Command2
    {0xFF, (uint8_t []){0x77, 0x01, 0x00, 0x00, 0x13}, 5, 0},
    // Unknown
    {0xEF, (uint8_t []){0x08}, 1, 0},

    // Command2 BK0 Selection: Enable the BK function of Command2
    {0xFF, (uint8_t []){0x77, 0x01, 0x00, 0x00, 0x10}, 5, 0},
    // Display Line Setting
    {0xC0, (uint8_t []){0x4f, 0x00}, 2, 0},
    // Porch Control
    {0xC1, (uint8_t []){0x10, 0x0c}, 2, 0},
    // Inversion selection & Frame Rate Control
    {0xC2, (uint8_t []){0x07, 0x14}, 2, 0},
    // Unknown
    {0xCC, (uint8_t []){0x10}, 1, 0},
    // Positive Voltage Gamma Control
    {0xB0, (uint8_t []){0x0a, 0x18, 0x1e, 0x12, 0x16, 0x0c, 0x0e, 0x0d, 0x0c, 0x29, 0x06, 0x14, 0x13, 0x29, 0x33, 0x1c}, 16, 0},
    // Negative Voltage Gamma Control
    {0xB1, (uint8_t []){0x0a, 0x19, 0x21, 0x0a, 0x0c, 0x00, 0x0c, 0x03, 0x03, 0x23, 0x01, 0x0e, 0x0c, 0x27, 0x2b, 0x1c}, 16, 0},

    // Command2 BK1 Selection: Enable the BK function of Command2
    {0xFF, (uint8_t []){0x77, 0x01, 0x00, 0x00, 0x11}, 5, 0},
    // Vop Amplitude setting
    {0xB0, (uint8_t []){0x5d}, 1, 0},
    // VCOM amplitude setting
    {0xB1, (uint8_t []){0x61}, 1, 0},
    // VGH Voltage setting
    {0xB2, (uint8_t []){0x84}, 1, 0},
    // TEST Command Setting
    {0xB3, (uint8_t []){0x80}, 1, 0},
    // VGL Voltage setting
    {0xB5, (uint8_t []){0x4d}, 1, 0},
    // Power Control 1
    {0xB7, (uint8_t []){0x85}, 1, 0},
    // Power Control 2
    {0xB8, (uint8_t []){0x20}, 1, 0},
    // Source pre_drive timing set1
    {0xC1, (uint8_t []){0x78}, 1, 0},
    // Source EQ2 Setting
    {0xC2, (uint8_t []){0x78}, 1, 0},
    // MIPI Setting 1
    {0xD0, (uint8_t []){0x88}, 1, 0},
    // GIP Code
    {0xE0, (uint8_t []){0x00, 0x00, 0x02}, 3, 0},
    {0xE1, (uint8_t []){0x06, 0xa0, 0x08, 0xa0, 0x05, 0xa0, 0x07, 0xa0, 0x00, 0x44, 0x44}, 11, 0},
    {0xE2, (uint8_t []){0x20, 0x20, 0x44, 0x44, 0x96, 0xa0, 0x00, 0x00, 0x96, 0xa0, 0x00, 0x00}, 12, 0},
    {0xE3, (uint8_t []){0x00, 0x00, 0x22, 0x22}, 4, 0},
    {0xE4, (uint8_t []){0x44, 0x44}, 2, 0},
    {0xE5, (uint8_t []){0x0d, 0x91, 0xa0, 0xa0, 0x0f, 0x93, 0xa0, 0xa0, 0x09, 0x8d, 0xa0, 0xa0, 0x0b, 0x8f, 0xa0, 0xa0}, 16, 0},
    {0xE6, (uint8_t []){0x00, 0x00, 0x22, 0x22}, 4, 0},
    {0xE7, (uint8_t []){0x44, 0x44}, 2, 0},
    {0xE8, (uint8_t []){0x0c, 0x90, 0xa0, 0xa0, 0x0e, 0x92, 0xa0, 0xa0, 0x08, 0x8c, 0xa0, 0xa0, 0x0a, 0x8e, 0xa0, 0xa0}, 16, 0},
    {0xE9, (uint8_t []){0x36, 0x00}, 2, 0},
    {0xEB, (uint8_t []){0x00, 0x01, 0xe4, 0xe4, 0x44, 0x88, 0x40}, 7, 0},
    {0xED, (uint8_t []){0xff, 0x45, 0x67, 0xfa, 0x01, 0x2b, 0xcf, 0xff, 0xff, 0xfc, 0xb2, 0x10, 0xaf, 0x76, 0x54, 0xff}, 16, 0},
    {0xEF, (uint8_t []){0x10, 0x0d, 0x04, 0x08, 0x3f, 0x1f}, 6, 0},

    // Command2 BK3 Selection: Enable the BK function of Command2
    // {0xFF, (uint8_t []){0x77, 0x01, 0x00, 0x00, 0x13}, 5, 0},
    // {0xE8, (uint8_t []){0x00, 0x0E}, 2, 0},
    // {0xE8, (uint8_t []){0x00, 0x0C}, 2, 20},
    // {0xE8, (uint8_t []){0x00, 0x00}, 2, 0},

    // Command2 BK0 Selection: Disable the BK function of Command2
    {0xFF, (uint8_t []){0x77, 0x01, 0x00, 0x00, 0x00}, 5, 0},
};

esp_io_expander_handle_t io_expander = NULL;

#define PCA9536_BACKLIGHT  IO_EXPANDER_PIN_NUM_1
#define PCA9536_USER_LED   IO_EXPANDER_PIN_NUM_2
#define PCA9536_RESET      IO_EXPANDER_PIN_NUM_3

void example_init_pca9536() {
    i2c_master_bus_handle_t i2c_handle = NULL;
    const i2c_master_bus_config_t bus_config = {
        .i2c_port = I2C_NUM_0,
        .sda_io_num = GPIO_NUM_7,
        .scl_io_num = GPIO_NUM_8,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .flags.enable_internal_pullup = true,
    };
    ESP_RETURN_VOID_ON_ERROR(
        i2c_new_master_bus(&bus_config, &i2c_handle), 
        TAG, "PCA9536: i2c bus init failed"
    );
    ESP_RETURN_VOID_ON_ERROR(
        esp_io_expander_new_i2c_pca9536(i2c_handle, ESP_IO_EXPANDER_I2C_PCA9536_ADDRESS, &io_expander), 
        TAG, "PCA9536: init failed"
    );
    ESP_LOGI(TAG, "PCA9536: Init");
}

void example_reset_pca9536() {
    // Set mode
    ESP_RETURN_VOID_ON_ERROR(
        esp_io_expander_set_dir(io_expander, PCA9536_BACKLIGHT | PCA9536_USER_LED, IO_EXPANDER_OUTPUT), 
        TAG, "PCA9536: set BL & UL mode output failed"
    );

    // Reset
    ESP_RETURN_VOID_ON_ERROR(
        esp_io_expander_set_dir(io_expander, PCA9536_RESET, IO_EXPANDER_OUTPUT), 
        TAG, "PCA9536: set RST mode output failed"
    );
    ESP_RETURN_VOID_ON_ERROR(
        esp_io_expander_set_level(io_expander, PCA9536_RESET, 0),
        TAG, "PCA9536: set RST0 failed"
    );
    vTaskDelay(pdMS_TO_TICKS(10));
    ESP_RETURN_VOID_ON_ERROR(
        esp_io_expander_set_level(io_expander, PCA9536_RESET, 1),
        TAG, "PCA9536: set RST1 failed"
    );
    vTaskDelay(pdMS_TO_TICKS(10));
    ESP_RETURN_VOID_ON_ERROR(
        esp_io_expander_set_level(io_expander, PCA9536_RESET, 0),
        TAG, "PCA9536: set RST0 failed"
    );
    ESP_RETURN_VOID_ON_ERROR(
        esp_io_expander_set_dir(io_expander, PCA9536_RESET, IO_EXPANDER_INPUT), 
        TAG, "PCA9536: set RST mode input failed"
    );
    vTaskDelay(pdMS_TO_TICKS(120));

    // Backlight & User LED
    ESP_RETURN_VOID_ON_ERROR(
        esp_io_expander_set_level(io_expander, PCA9536_BACKLIGHT | PCA9536_USER_LED, 1),
        TAG, "PCA9536: BL & UL on failed"
    );

    ESP_LOGI(TAG, "PCA9536: Reset");
}
#endif

void app_main(void)
{
#if CONFIG_EXAMPLE_MONITOR_REFRESH_BY_GPIO
    example_bsp_init_refresh_monitor_io();
#endif

    example_bsp_enable_dsi_phy_power();
    example_bsp_init_lcd_backlight();
    example_bsp_set_lcd_backlight(EXAMPLE_LCD_BK_LIGHT_OFF_LEVEL);

    // create MIPI DSI bus first, it will initialize the DSI PHY as well
    esp_lcd_dsi_bus_handle_t mipi_dsi_bus;
    esp_lcd_dsi_bus_config_t bus_config = {
        .bus_id = 0,
        .num_data_lanes = EXAMPLE_MIPI_DSI_LANE_NUM,
        .phy_clk_src = MIPI_DSI_PHY_CLK_SRC_DEFAULT,
        .lane_bit_rate_mbps = EXAMPLE_MIPI_DSI_LANE_BITRATE_MBPS,
    };
    ESP_ERROR_CHECK(esp_lcd_new_dsi_bus(&bus_config, &mipi_dsi_bus));

    ESP_LOGI(TAG, "Install MIPI DSI LCD control IO");
    esp_lcd_panel_io_handle_t mipi_dbi_io;
    // we use DBI interface to send LCD commands and parameters
    esp_lcd_dbi_io_config_t dbi_config = {
        .virtual_channel = 0,
        .lcd_cmd_bits = 8,   // according to the LCD spec
        .lcd_param_bits = 8, // according to the LCD spec
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_dbi(mipi_dsi_bus, &dbi_config, &mipi_dbi_io));

    ESP_LOGI(TAG, "Install MIPI DSI LCD data panel");
    esp_lcd_panel_handle_t mipi_dpi_panel = NULL;
    esp_lcd_dpi_panel_config_t dpi_config = {
        .virtual_channel = 0,
        .dpi_clk_src = MIPI_DSI_DPI_CLK_SRC_DEFAULT,
        .dpi_clock_freq_mhz = EXAMPLE_MIPI_DSI_DPI_CLK_MHZ,
        .pixel_format = LCD_COLOR_PIXEL_FORMAT_RGB888,
        .in_color_format = LCD_COLOR_FMT_RGB888,
        .out_color_format = LCD_COLOR_FMT_RGB888,
        .video_timing = {
            .h_size = EXAMPLE_MIPI_DSI_LCD_H_RES,
            .v_size = EXAMPLE_MIPI_DSI_LCD_V_RES,
            .hsync_back_porch = EXAMPLE_MIPI_DSI_LCD_HBP,
            .hsync_pulse_width = EXAMPLE_MIPI_DSI_LCD_HSYNC,
            .hsync_front_porch = EXAMPLE_MIPI_DSI_LCD_HFP,
            .vsync_back_porch = EXAMPLE_MIPI_DSI_LCD_VBP,
            .vsync_pulse_width = EXAMPLE_MIPI_DSI_LCD_VSYNC,
            .vsync_front_porch = EXAMPLE_MIPI_DSI_LCD_VFP,
        },
#if CONFIG_EXAMPLE_USE_DMA2D_COPY_FRAME
        .flags.use_dma2d = true, // use DMA2D to copy draw buffer into frame buffer
#endif
    };

#if CONFIG_EXAMPLE_LCD_USE_ILI9881C
    ili9881c_vendor_config_t vendor_config = {
        .mipi_config = {
            .dsi_bus = mipi_dsi_bus,
            .dpi_config = &dpi_config,
            .lane_num = EXAMPLE_MIPI_DSI_LANE_NUM,
        },
    };
    esp_lcd_panel_dev_config_t lcd_dev_config = {
        .reset_gpio_num = EXAMPLE_PIN_NUM_LCD_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 24,
        .vendor_config = &vendor_config,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_ili9881c(mipi_dbi_io, &lcd_dev_config, &mipi_dpi_panel));
#elif CONFIG_EXAMPLE_LCD_USE_EK79007
    ek79007_vendor_config_t vendor_config = {
        .mipi_config = {
            .dsi_bus = mipi_dsi_bus,
            .dpi_config = &dpi_config,
        },
    };
    esp_lcd_panel_dev_config_t lcd_dev_config = {
        .reset_gpio_num = EXAMPLE_PIN_NUM_LCD_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 24,
        .vendor_config = &vendor_config,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_ek79007(mipi_dbi_io, &lcd_dev_config, &mipi_dpi_panel));
#elif CONFIG_EXAMPLE_LCD_USE_ST7701
    st7701_vendor_config_t vendor_config = {
        .mipi_config = {
            .dsi_bus = mipi_dsi_bus,
            .dpi_config = &dpi_config,
        },
        .flags = {
            .use_mipi_interface = 1,
        },
        .init_cmds = lcd_init_cmds,
        .init_cmds_size = sizeof(lcd_init_cmds) / sizeof(st7701_lcd_init_cmd_t),
    };

    esp_lcd_panel_dev_config_t lcd_dev_config = {
        .reset_gpio_num = EXAMPLE_PIN_NUM_LCD_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 24,
        .vendor_config = &vendor_config,
    };

    example_init_pca9536();
    example_reset_pca9536();

    ESP_ERROR_CHECK(esp_lcd_new_panel_st7701(mipi_dbi_io, &lcd_dev_config, &mipi_dpi_panel));
#endif

    ESP_ERROR_CHECK(esp_lcd_panel_reset(mipi_dpi_panel));
    ESP_ERROR_CHECK(esp_lcd_panel_init(mipi_dpi_panel));

#if CONFIG_EXAMPLE_LCD_USE_ST7701
    ESP_ERROR_CHECK(esp_lcd_panel_disp_sleep(mipi_dpi_panel, false));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(mipi_dpi_panel, true));
#endif

    // turn on backlight
    example_bsp_set_lcd_backlight(EXAMPLE_LCD_BK_LIGHT_ON_LEVEL);

    ESP_LOGI(TAG, "Initialize LVGL library");
    lv_init();
    // create a lvgl display
    lv_display_t *display = lv_display_create(EXAMPLE_MIPI_DSI_LCD_H_RES, EXAMPLE_MIPI_DSI_LCD_V_RES);
    // associate the mipi panel handle to the display
    lv_display_set_user_data(display, mipi_dpi_panel);
    // set color depth
    lv_display_set_color_format(display, LV_COLOR_FORMAT_RGB888);
    // create draw buffer
    void *buf1 = NULL;
    void *buf2 = NULL;
    ESP_LOGI(TAG, "Allocate separate LVGL draw buffers");
    // Note:
    // Keep the display buffer in **internal** RAM can speed up the UI because LVGL uses it a lot and it should have a fast access time
    // This example allocate the buffer from PSRAM mainly because we want to save the internal RAM
    size_t draw_buffer_sz = EXAMPLE_MIPI_DSI_LCD_H_RES * EXAMPLE_LVGL_DRAW_BUF_LINES * sizeof(lv_color_t);
    buf1 = heap_caps_malloc(draw_buffer_sz, MALLOC_CAP_SPIRAM);
    assert(buf1);
    buf2 = heap_caps_malloc(draw_buffer_sz, MALLOC_CAP_SPIRAM);
    assert(buf2);
    // initialize LVGL draw buffers
    lv_display_set_buffers(display, buf1, buf2, draw_buffer_sz, LV_DISPLAY_RENDER_MODE_PARTIAL);
    // set the callback which can copy the rendered image to an area of the display
    lv_display_set_flush_cb(display, example_lvgl_flush_cb);

    ESP_LOGI(TAG, "Register DPI panel event callback for LVGL flush ready notification");
    esp_lcd_dpi_panel_event_callbacks_t cbs = {
        .on_color_trans_done = example_notify_lvgl_flush_ready,
#if CONFIG_EXAMPLE_MONITOR_REFRESH_BY_GPIO
        .on_refresh_done = example_monitor_refresh_rate,
#endif
    };
    ESP_ERROR_CHECK(esp_lcd_dpi_panel_register_event_callbacks(mipi_dpi_panel, &cbs, display));

    ESP_LOGI(TAG, "Use esp_timer as LVGL tick timer");
    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = &example_increase_lvgl_tick,
        .name = "lvgl_tick"
    };
    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, EXAMPLE_LVGL_TICK_PERIOD_MS * 1000));

    ESP_LOGI(TAG, "Create LVGL task");
    xTaskCreate(example_lvgl_port_task, "LVGL", EXAMPLE_LVGL_TASK_STACK_SIZE, NULL, EXAMPLE_LVGL_TASK_PRIORITY, NULL);

    ESP_LOGI(TAG, "Display LVGL Meter Widget");
    _lock_acquire(&lvgl_api_lock);
    example_lvgl_demo_ui(display);
    _lock_release(&lvgl_api_lock);
}
