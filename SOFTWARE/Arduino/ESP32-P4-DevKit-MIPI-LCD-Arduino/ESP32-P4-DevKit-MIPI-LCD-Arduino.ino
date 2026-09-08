/*
 * Olimex ESP32-P4-DevKit + MIPI-LCD2.8-640x480 Arduino LVGL test.
 *
 * This keeps the LCD init path from Olimex's ESP-IDF demo, but wraps it as a
 * normal Arduino sketch.
 *
 * Required Arduino IDE Tools menu settings (select these before compiling):
 *   Board:             ESP32P4 Dev Module
 *   USB Mode:          Hardware CDC and JTAG
 *   USB CDC On Boot:   Enabled (required for the Serial Monitor diagnostics)
 *   Upload Mode:       UART0 / Hardware CDC
 *   Flash Frequency:   80 MHz
 *   Flash Mode:        DIO
 *   Flash Size:        16 MB (128 Mb) -- do not use a smaller flash setting
 *   PSRAM:             Enabled -- REQUIRED for the 480x640 RGB888 framebuffer
 *   Chip Variant:      Before v3.00 for ESP32-P4 revision 1.3 boards
 *
 * With PSRAM disabled, the MIPI driver cannot allocate its 921,600-byte frame
 * buffer and startup stops with "lcd.dsi: no memory for frame buffer".
 *
 * Serial logging is enabled at 115200 baud. On Windows, copy
 * arduino-esp32-boards.local.txt as boards.local.txt before using Serial
 * Monitor; without it, closing Serial Monitor can reset the board through
 * DTR/RTS. Set OLIMEX_DEBUG_SERIAL to 0 to disable sketch logging.
 *
 * Select the display in esp_panel_board_custom_conf.h:
 *   OLIMEX_MIPI_LCD_VERSION 2: WLK2802MIPI-15P-V2 (default; no PCA9536)
 *   OLIMEX_MIPI_LCD_VERSION 1: WLK2802MIPI-15P (PCA9536 reset/backlight)
 */

#include <assert.h>
#include <stdarg.h>
#include <Arduino.h>
#include <esp_heap_caps.h>
#include <esp_display_panel.hpp>
#include <lvgl.h>

#include "esp_panel_board_custom_conf.h"
#include "lvgl_v8_port.h"

using namespace esp_panel::board;
using namespace esp_panel::drivers;

// These must be macros: enum names are not evaluated by preprocessor #if.
#define TEST_MODE_DSI_PATTERN 1
#define TEST_MODE_SOFTWARE_COLOR_BARS 2
#define TEST_MODE_LVGL_UI 3

#ifndef OLIMEX_LCD_TEST_MODE
#define OLIMEX_LCD_TEST_MODE TEST_MODE_LVGL_UI
#endif

#ifndef OLIMEX_DEBUG_SERIAL
#define OLIMEX_DEBUG_SERIAL 1
#endif

#if OLIMEX_LCD_TEST_MODE < 1 || OLIMEX_LCD_TEST_MODE > 3
#error "Invalid OLIMEX_LCD_TEST_MODE"
#endif

static Board *board = nullptr;

static lv_obj_t *ui_screen = nullptr;
static lv_obj_t *ui_title = nullptr;
static lv_obj_t *ui_subtitle = nullptr;
static lv_obj_t *ui_status = nullptr;
static lv_obj_t *ui_versions = nullptr;
static lv_obj_t *ui_bar = nullptr;

static void debug_begin()
{
#if OLIMEX_DEBUG_SERIAL
    Serial.begin(115200);
#if ARDUINO_USB_CDC_ON_BOOT
    uint32_t start_ms = millis();
    while (!Serial && ((millis() - start_ms) < 2000)) {
        delay(10);
    }
#endif
#endif
}

static void debug_printf(const char *format, ...)
{
#if OLIMEX_DEBUG_SERIAL
    char line[192];
    va_list args;
    va_start(args, format);
    vsnprintf(line, sizeof(line), format, args);
    va_end(args);

    Serial.print(line);
#else
    (void)format;
#endif
}

static void halt_with_message(const char *message)
{
    debug_printf("%s\n", message);
    while (true) {
        delay(1000);
    }
}

static void animate_bar(void *bar, int32_t value)
{
    lv_bar_set_value(static_cast<lv_obj_t *>(bar), value, LV_ANIM_OFF);
}

static void apply_ui_palette(lv_color_t background)
{
    lv_color32_t rgb = {.full = lv_color_to32(background)};
    const uint16_t brightness = (299U * rgb.ch.red + 587U * rgb.ch.green + 114U * rgb.ch.blue) / 1000U;
    const bool is_light_background = brightness > 145;

    const lv_color_t primary = lv_color_hex(is_light_background ? 0x1B2430 : 0xF7F4E9);
    const lv_color_t secondary = lv_color_hex(is_light_background ? 0x235B59 : 0x8ED1C6);
    const lv_color_t emphasis = lv_color_hex(is_light_background ? 0x5C3A75 : 0xFFD166);
    const lv_color_t muted = lv_color_hex(is_light_background ? 0x33404A : 0xB7C3CC);
    const lv_color_t bar_background = lv_color_hex(is_light_background ? 0xF7F4E9 : 0x23313B);
    const lv_color_t bar_indicator = lv_color_hex(is_light_background ? 0x5C3A75 : 0x8ED1C6);

    // A palette change touches several objects. Suppress LVGL's intermediate
    // invalidations, then redraw the screen only once. Without this, a color
    // change can trigger several consecutive full-screen RGB888 transfers.
    lv_obj_enable_style_refresh(false);
    lv_obj_set_style_bg_color(ui_screen, background, 0);
    lv_obj_set_style_text_color(ui_title, primary, 0);
    lv_obj_set_style_text_color(ui_subtitle, secondary, 0);
    lv_obj_set_style_text_color(ui_status, emphasis, 0);
    lv_obj_set_style_text_color(ui_versions, muted, 0);
    lv_obj_set_style_bg_color(ui_bar, bar_background, LV_PART_MAIN);
    lv_obj_set_style_bg_color(ui_bar, bar_indicator, LV_PART_INDICATOR);
    lv_obj_enable_style_refresh(true);
    lv_obj_invalidate(ui_screen);
}

static void change_background(lv_timer_t *)
{
    // Use one solid update rather than a full-screen fade. A fade transfers the
    // RGB888 framebuffer in strips and can be seen as a band moving down the panel.
    static const uint32_t colors[] = {
        0x000000, // black
        0x4A4A4A, // grey
        0xB9EBC5, // light green
        0xD5C0F0, // light purple
        0xFFC1D6, // light pink
        0x243B55, // deep blue
    };
    static uint8_t color_index = 1;

    apply_ui_palette(lv_color_hex(colors[color_index]));
    color_index = (color_index + 1) % (sizeof(colors) / sizeof(colors[0]));
}

static void create_test_ui()
{
    ui_screen = lv_scr_act();
    lv_obj_set_style_bg_opa(ui_screen, LV_OPA_COVER, 0);

    ui_title = lv_label_create(ui_screen);
    lv_label_set_text(ui_title, "Olimex ESP32-P4");
    lv_obj_set_style_text_font(ui_title, &lv_font_montserrat_28, 0);
    lv_obj_align(ui_title, LV_ALIGN_CENTER, 0, -70);

    ui_subtitle = lv_label_create(ui_screen);
    lv_label_set_text(ui_subtitle, "MIPI-LCD2.8 ST7701 Arduino");
    lv_obj_set_style_text_font(ui_subtitle, &lv_font_montserrat_16, 0);
    lv_obj_align_to(ui_subtitle, ui_title, LV_ALIGN_OUT_BOTTOM_MID, 0, 12);

    ui_status = lv_label_create(ui_screen);
    lv_label_set_text(ui_status, "Display init OK");
    lv_obj_set_style_text_font(ui_status, &lv_font_montserrat_20, 0);
    lv_obj_align(ui_status, LV_ALIGN_CENTER, 0, 35);

    ui_bar = lv_bar_create(ui_screen);
    lv_obj_set_size(ui_bar, 260, 18);
    lv_bar_set_range(ui_bar, 0, 100);
    lv_bar_set_value(ui_bar, 0, LV_ANIM_OFF);
    lv_obj_align_to(ui_bar, ui_status, LV_ALIGN_OUT_BOTTOM_MID, 0, 24);

    lv_anim_t animation;
    lv_anim_init(&animation);
    lv_anim_set_var(&animation, ui_bar);
    lv_anim_set_exec_cb(&animation, animate_bar);
    lv_anim_set_values(&animation, 0, 100);
    lv_anim_set_time(&animation, 1800);
    lv_anim_set_playback_time(&animation, 1800);
    lv_anim_set_repeat_count(&animation, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_path_cb(&animation, lv_anim_path_ease_in_out);
    lv_anim_start(&animation);

    ui_versions = lv_label_create(ui_screen);
    lv_label_set_text_fmt(
        ui_versions,
        "ESP32_Display_Panel %d.%d.%d  |  LVGL %d.%d.%d",
        ESP_PANEL_VERSION_MAJOR, ESP_PANEL_VERSION_MINOR, ESP_PANEL_VERSION_PATCH,
        LVGL_VERSION_MAJOR, LVGL_VERSION_MINOR, LVGL_VERSION_PATCH
    );
    lv_obj_set_style_text_font(ui_versions, &lv_font_montserrat_16, 0);
    lv_obj_align(ui_versions, LV_ALIGN_BOTTOM_MID, 0, -30);

    apply_ui_palette(lv_color_hex(0x000000));
    lv_timer_create(change_background, 10000, nullptr);
}

void setup()
{
    debug_begin();
    delay(500);
    debug_printf("\n");
    debug_printf("Olimex ESP32-P4-DevKit MIPI-LCD2.8 Arduino LCD test\n");
    debug_printf("Mode %d: %s\n", OLIMEX_LCD_TEST_MODE,
                 (OLIMEX_LCD_TEST_MODE == TEST_MODE_DSI_PATTERN) ? "DSI hardware pattern" :
                 (OLIMEX_LCD_TEST_MODE == TEST_MODE_SOFTWARE_COLOR_BARS) ? "software color bars" :
                 "LVGL UI");
    debug_printf("Arduino USB CDC on boot: %d, USB mode: %d\n", ARDUINO_USB_CDC_ON_BOOT, ARDUINO_USB_MODE);
    debug_printf("LVGL color depth: %d, lv_color_t size: %u\n", LV_COLOR_DEPTH, (unsigned)sizeof(lv_color_t));
#if OLIMEX_MIPI_LCD_VERSION == 1
    debug_printf("V1 LCD: PCA9536 reset/backlight enabled; RGB565 LVGL converted to RGB888 display data\n");
#else
    debug_printf("V2 LCD: no PCA9536; RGB565 LVGL converted to RGB888 display data\n");
#endif
    debug_printf("PSRAM: total=%u free=%u; internal free=%u\n",
                 ESP.getPsramSize(), heap_caps_get_free_size(MALLOC_CAP_SPIRAM),
                 heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
    debug_printf("MIPI DSI config: lanes=%d, lane_rate=%d Mbps, dpi_clk=%d MHz, dpi_bits=%d\n",
                 ESP_PANEL_BOARD_LCD_MIPI_DSI_LANE_NUM,
                 ESP_PANEL_BOARD_LCD_MIPI_DSI_LANE_RATE_MBPS,
                 ESP_PANEL_BOARD_LCD_MIPI_DPI_CLK_MHZ,
                 ESP_PANEL_BOARD_LCD_MIPI_DPI_PIXEL_BITS);

    static_assert(LV_COLOR_DEPTH == 16, "This sketch expects LVGL RGB565");
    static_assert(sizeof(lv_color_t) == 2, "This sketch expects 16-bit lv_color_t");

    debug_printf("Initializing board\n");
    board = new Board();
    if ((board == nullptr) || !board->init()) {
        halt_with_message("Board init failed");
    }

#if LVGL_PORT_AVOID_TEARING_MODE
    auto lcd = board->getLCD();
    lcd->configFrameBufferNumber(LVGL_PORT_DISP_BUFFER_NUM);

#if ESP_PANEL_DRIVERS_BUS_ENABLE_RGB && CONFIG_IDF_TARGET_ESP32S3
    auto lcd_bus = lcd->getBus();
    if (lcd_bus->getBasicAttributes().type == ESP_PANEL_BUS_TYPE_RGB) {
        static_cast<BusRGB *>(lcd_bus)->configRGB_BounceBufferSize(lcd->getFrameWidth() * 10);
    }
#endif
#endif

    debug_printf("Beginning board\n");
    if (!board->begin()) {
        halt_with_message("Board begin failed");
    }

    auto lcd = board->getLCD();
    debug_printf("LCD frame: %dx%d, color bits: %d\n",
                 lcd->getFrameWidth(), lcd->getFrameHeight(), lcd->getFrameColorBits());

#if OLIMEX_LCD_TEST_MODE == TEST_MODE_DSI_PATTERN
    debug_printf("Showing MIPI DSI hardware color-bar pattern\n");
    if (!lcd->DSI_ColorBarPatternTest(LCD::DSI_ColorBarPattern::BAR_VERTICAL)) {
        halt_with_message("DSI pattern test failed");
    }
    debug_printf("DSI pattern is running; LVGL is not started in this mode\n");
    return;
#elif OLIMEX_LCD_TEST_MODE == TEST_MODE_SOFTWARE_COLOR_BARS
    debug_printf("Drawing software color bars through LCD::drawBitmap\n");
    if (!lcd->colorBarTest()) {
        halt_with_message("Software color-bar test failed");
    }
    debug_printf("Software color bars drawn; LVGL is not started in this mode\n");
    return;
#else
    debug_printf("Initializing LVGL\n");
    if (!lvgl_port_init(board->getLCD(), board->getTouch())) {
        halt_with_message("LVGL init failed");
    }

    debug_printf("Creating UI\n");
    lvgl_port_lock(-1);
    create_test_ui();
    lvgl_port_unlock();

    debug_printf("LCD initialized\n");
#endif
}

void loop()
{
    static uint32_t pattern_index = 0;

#if OLIMEX_DEBUG_SERIAL
    static uint32_t last_print_ms = 0;
    if ((millis() - last_print_ms) >= 2000) {
        last_print_ms = millis();
        debug_printf("alive, mode %d, uptime %lu ms\n", OLIMEX_LCD_TEST_MODE, (unsigned long)millis());
    }
#endif

#if OLIMEX_LCD_TEST_MODE == TEST_MODE_DSI_PATTERN
    if ((board != nullptr) && ((millis() / 3000) != pattern_index)) {
        pattern_index = millis() / 3000;
        auto lcd = board->getLCD();
        switch (pattern_index % 3) {
        case 0:
            lcd->DSI_ColorBarPatternTest(LCD::DSI_ColorBarPattern::BAR_VERTICAL);
            break;
        case 1:
            lcd->DSI_ColorBarPatternTest(LCD::DSI_ColorBarPattern::BAR_HORIZONTAL);
            break;
        default:
            lcd->DSI_ColorBarPatternTest(LCD::DSI_ColorBarPattern::BER_VERTICAL);
            break;
        }
    }
#endif

    delay(50);
}
