/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "esp_check.h"
#include "esp_display_panel.hpp"
#include "esp_lib_utils.h"
#include "lvgl.h"
#include "lvgl_v8_port.h"
#include "lv_demos.h"
#include "BOARD_OLIMEX_ESP32_P4_DEVKIT.h"

#if OLIMEX_MIPI_LCD_VERSION == 1
#include "driver/i2c_master.h"
#endif

using namespace esp_panel::drivers;
using namespace esp_panel::board;

static const char *TAG = "example";

#if OLIMEX_MIPI_LCD_VERSION == 1
static void init_v1_pca9536_lcd_control()
{
    i2c_master_bus_handle_t i2c_bus = NULL;
    i2c_master_dev_handle_t pca9536 = NULL;
    i2c_master_bus_config_t bus_config = {};
    bus_config.i2c_port = I2C_NUM_0;
    bus_config.sda_io_num = GPIO_NUM_7;
    bus_config.scl_io_num = GPIO_NUM_8;
    bus_config.clk_source = I2C_CLK_SRC_DEFAULT;
    bus_config.flags.enable_internal_pullup = true;
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &i2c_bus));
    i2c_device_config_t device_config = {};
    device_config.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    device_config.device_address = 0x41;
    device_config.scl_speed_hz = 400000;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_bus, &device_config, &pca9536));

    const auto write_pca9536 = [pca9536](uint8_t reg, uint8_t value) {
        const uint8_t data[] = {reg, value};
        ESP_ERROR_CHECK(i2c_master_transmit(pca9536, data, sizeof(data), -1));
    };
    constexpr uint8_t PCA9536_OUTPUT = 0x01;
    constexpr uint8_t PCA9536_CONFIG = 0x03;
    write_pca9536(PCA9536_CONFIG, 0x01);
    write_pca9536(PCA9536_OUTPUT, 0x00);
    vTaskDelay(pdMS_TO_TICKS(10));
    write_pca9536(PCA9536_OUTPUT, 0x08);
    vTaskDelay(pdMS_TO_TICKS(10));
    write_pca9536(PCA9536_OUTPUT, 0x00);
    write_pca9536(PCA9536_CONFIG, 0x09);
    vTaskDelay(pdMS_TO_TICKS(120));
    write_pca9536(PCA9536_OUTPUT, 0x06);
    ESP_LOGI(TAG, "V1 LCD PCA9536 reset and backlight enabled");
}
#endif

extern "C" void app_main()
{
    Board *board = new Board();
    assert(board);

    ESP_LOGI(TAG, "Initializing board");
    ESP_UTILS_CHECK_FALSE_EXIT(board->init(), "Board init failed");    

#if OLIMEX_MIPI_LCD_VERSION == 1
    init_v1_pca9536_lcd_control();
#else
    ESP_LOGI(TAG, "V2 LCD: RGB888, no PCA9536 control");
#endif

#if LVGL_PORT_AVOID_TEARING_MODE
    auto lcd = board->getLCD();
    // When avoid tearing function is enabled, the frame buffer number should be set in the board driver
    lcd->configFrameBufferNumber(LVGL_PORT_DISP_BUFFER_NUM);
#if ESP_PANEL_DRIVERS_BUS_ENABLE_RGB && CONFIG_IDF_TARGET_ESP32S3
    auto lcd_bus = lcd->getBus();
    /**
     * As the anti-tearing feature typically consumes more PSRAM bandwidth, for the ESP32-S3, we need to utilize the
     * "bounce buffer" functionality to enhance the RGB data bandwidth.
     * This feature will consume `bounce_buffer_size * bytes_per_pixel * 2` of SRAM memory.
     */
    if (lcd_bus->getBasicAttributes().type == ESP_PANEL_BUS_TYPE_RGB) {
        static_cast<BusRGB *>(lcd_bus)->configRGB_BounceBufferSize(lcd->getFrameWidth() * 10);
    }
#endif
#endif
    ESP_UTILS_CHECK_FALSE_EXIT(board->begin(), "Board begin failed");
    
    ESP_LOGI(TAG, "Initializing LVGL");
    ESP_UTILS_CHECK_FALSE_EXIT(lvgl_port_init(board->getLCD(), board->getTouch()), "LVGL init failed");

    ESP_LOGI(TAG, "Creating UI");
    
    /* Lock the mutex due to the LVGL APIs are not thread-safe */
    lvgl_port_lock(-1);

    // lv_demo_widgets();
    // lv_demo_benchmark();
    lv_demo_music();
    // lv_demo_stress();

    /* Release the mutex */
    lvgl_port_unlock();
}
