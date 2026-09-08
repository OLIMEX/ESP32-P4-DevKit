/*
 * Keep the ESP32_Display_Panel built-in board selector disabled.
 * This sketch uses esp_panel_board_custom_conf.h for the Olimex wiring.
 */

#pragma once

#define ESP_PANEL_BOARD_DEFAULT_USE_SUPPORTED       (0)

#if ESP_PANEL_BOARD_DEFAULT_USE_SUPPORTED
#define ESP_PANEL_BOARD_SUPPORTED_FILE_VERSION_MAJOR 1
#define ESP_PANEL_BOARD_SUPPORTED_FILE_VERSION_MINOR 3
#define ESP_PANEL_BOARD_SUPPORTED_FILE_VERSION_PATCH 0
#endif
