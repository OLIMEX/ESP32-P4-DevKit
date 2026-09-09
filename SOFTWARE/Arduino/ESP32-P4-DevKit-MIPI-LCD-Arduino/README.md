# ESP32-P4-DevKit MIPI-LCD2.8 Arduino demo

Arduino/LVGL demo for the Olimex ESP32-P4-DevKit or ESP32-P4-PC and the
MIPI-LCD2.8-640x480 V1 (`WLK2802MIPI-15P`) or V2 (`WLK2802MIPI-15P-V2`)
display. It configures the ST7701 panel for one-lane MIPI DSI, renders an LVGL
status screen, and animates the progress bar continuously. The background
switches between six solid colors every 10 seconds, with text and bar colors
adjusted for contrast. GPIO2 drives the board user LED as a 1 Hz heartbeat.

The display path is RGB888. LVGL renders RGB565 and the local port converts
each updated area to RGB888 before it is copied to the MIPI DSI framebuffer.

## Requirements

- Olimex ESP32-P4-DevKit or ESP32-P4-PC, with the display connected to its
  **MIPI-DSI** connector.
- MIPI-LCD2.8-640x480 V1 or V2 display and its ribbon cable.
- Arduino IDE with **esp32 by Espressif Systems 3.3.8** installed through
  Boards Manager.
- These libraries installed through Library Manager:

| Library | Tested version |
| --- | --- |
| ESP32_Display_Panel | 1.0.4 |
| ESP32_IO_Expander | 1.1.1 |
| esp-lib-utils | 0.3.0 |
| lvgl | 8.4.0 |

Use LVGL 8.4.0. The project does not support LVGL 9.

## Arduino IDE settings

Open `ESP32-P4-DevKit-MIPI-LCD-Arduino.ino`, then select:

| Tools setting | Value |
| --- | --- |
| Board | ESP32P4 Dev Module |
| USB Mode | Hardware CDC and JTAG |
| USB CDC On Boot | **Enabled** |
| Upload Mode | UART0 / Hardware CDC |
| Flash Frequency | 80 MHz |
| Flash Mode | DIO |
| Flash Size | 16 MB |
| **PSRAM** | **Enabled — required** |
| Chip Variant | Before v3.00 for ESP32-P4 revision 1.3 |

`PSRAM: Enabled` is mandatory. The RGB888 480x640 MIPI DSI framebuffer needs
921,600 bytes of PSRAM. If PSRAM is disabled, startup stops with:

```text
lcd.dsi: no memory for frame buffer
Board begin failed
```

Use the appropriate chip variant if the board has later silicon.

## Upload and run

1. Power off before fitting or changing the LCD ribbon.
2. Connect the display to MIPI-DSI, power the board, and select its COM port.
3. Verify, then Upload from Arduino IDE.
4. Apply the Windows DTR/RTS override below before opening Serial Monitor.
5. Open Serial Monitor at **115200 baud**. Startup diagnostics and an `alive`
   message every two seconds confirm that the sketch is running.

Expected startup output includes:

```text
Mode 3: LVGL UI
V2 LCD: no PCA9536; RGB565 LVGL converted to RGB888 display data
PSRAM: total=33554432 free=...; internal free=...
LCD frame: 480x640, color bits: 24
Initializing LVGL
Creating UI
LCD initialized
```

## Prevent Serial Monitor resets (DTR/RTS)

On Windows, closing Arduino IDE's Serial Monitor can change the DTR/RTS control
lines and reset this ESP32-P4 board. The problem was not reproduced with the
same board, cable, and LED test on Linux.

Before using Serial Monitor, copy
`arduino-esp32-boards.local.txt` to the installed ESP32 Arduino-core directory
and rename it to `boards.local.txt`. For Arduino core 3.3.8 on Windows, that is:

```text
C:\Users\<your-user>\AppData\Local\Arduino15\packages\esp32\hardware\esp32\3.3.8\boards.local.txt
```

Restart Arduino IDE after copying it. The override makes its monitor default to
`DTR: off` and `RTS: off` for **ESP32P4 Dev Module**. It is retained when IDE
starts again, but must be copied again after updating the ESP32 board package.

Serial diagnostics are enabled by default. To make a silent build, set
`OLIMEX_DEBUG_SERIAL` to `0` near the top of the sketch and upload again.

The screen shows the Olimex title, display status, and a progress bar that
continuously fills and empties.

## Diagnostic modes

The default in the sketch is the animated LVGL interface:

```cpp
#define OLIMEX_LCD_TEST_MODE TEST_MODE_LVGL_UI
```

For link diagnosis, change that one definition to either:

```cpp
#define OLIMEX_LCD_TEST_MODE TEST_MODE_DSI_PATTERN
// or
#define OLIMEX_LCD_TEST_MODE TEST_MODE_SOFTWARE_COLOR_BARS
```

The DSI-pattern mode bypasses LVGL and cycles controller-generated patterns.
Software color bars exercise the framebuffer path without the LVGL UI.

## Select the display revision

The default is V2. In `esp_panel_board_custom_conf.h`, leave this line as `2`:

```cpp
#define OLIMEX_MIPI_LCD_VERSION (2)
```

For the original V1 display, change it to `1`, then compile and upload again:

```cpp
#define OLIMEX_MIPI_LCD_VERSION (1)
```

V1 enables the PCA9536 I2C expander at address `0x41` for the documented reset
sequence and backlight control. V2 leaves that expander disabled because it is
not populated. Both revisions use the same ST7701 MIPI-DSI RGB888 display path.
Both selections compile with the listed Arduino dependencies; V2 was verified
on the connected display.

## Notes and limitations

- Touch is not implemented.
- The display uses one DSI lane at 500 Mbps and a 16 MHz DPI clock, as used by
  the working V2 test.

## User LED heartbeat

GPIO2 drives the user LED with a 500 ms on / 500 ms off heartbeat. This is
available on both the ESP32-P4-DevKit and ESP32-P4-PC, and gives a quick
indication that the sketch is still running even when Serial Monitor is closed.

The sketch assumes an active-high LED. If a board revision uses an active-low
LED, set this near the top of the `.ino` file before compiling:

```cpp
#define OLIMEX_USER_LED_ACTIVE_LEVEL LOW
```

## Project files

- `arduino-esp32-boards.local.txt` - Windows Serial Monitor DTR/RTS override.
- `ESP32-P4-DevKit-MIPI-LCD-Arduino.ino` — application and animated UI.
- `esp_panel_board_custom_conf.h` — V1/V2 selection, ST7701 init sequence,
  MIPI DSI timing, and the V1 expander setup.
- `esp_panel_drivers_conf.h` — enabled ESP32_Display_Panel drivers.
- `lvgl_v8_port.cpp` / `lvgl_v8_port.h` — LVGL display port and RGB565-to-RGB888
  conversion.
- `lv_conf.h` — project-local LVGL 8 configuration.

Do not commit generated `build*` directories, `.elf`, `.bin`, `.o`, or Arduino
IDE temporary files. See the repository `.gitignore` for the generated-output
patterns.
