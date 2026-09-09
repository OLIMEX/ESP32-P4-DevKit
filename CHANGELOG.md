# Changelog

## MIPI-LCD2.8 V1/V2 revision

### Changed

- `SOFTWARE/Demo_Examples/mipi_dsi` now defaults to the WLK2802MIPI-15P-V2 LCD: RGB888 data, one DSI lane at 500 Mbps, 16 MHz DPI clock, and no PCA9536 access.
- `SOFTWARE/Demo_Examples/lvgl_v8_port` now uses the same V2 defaults and converts its RGB565 LVGL draw data to RGB888 before writing it to the LCD frame buffer.
- Both projects provide the `OLIMEX_MIPI_LCD_VERSION` selector. Set it to `1` for the original WLK2802MIPI-15P display.
- V1 enables the PCA9536 at I2C address `0x41` for the display reset and backlight sequence, and uses RGB565, 1000 Mbps DSI, and a 25 MHz DPI clock.

### Added

- `mipi_dsi/sdkconfig.defaults.v1` supplies the matching LVGL RGB565 default for V1 builds.
- Project readmes explain the two LCD revisions, the default configuration, and the V1 build step.
- Repository `.gitignore` excludes ESP-IDF build output, downloaded `managed_components`, and generated `sdkconfig.old` backups.

### Compatibility

- V2 is the default configuration and is the configuration validated with the current MIPI-LCD2.8 display.
- V1 remains available for original displays with the PCA9536 controller.

