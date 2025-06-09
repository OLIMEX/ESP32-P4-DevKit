| Supported ESP SoCs | ESP32-P4 |
| ------------------ | ----- |

# LVGL (v8) Porting Example

## Overview

This example demonstrates how to stream camera input to the display. It requires camera, display, and ESP32-P4 board.

## How to use

### ESP-IDF Required

* The ESP-IDF TAG `v5.1` or later is required to use this example. For using the branch of ESP-IDF, the latest branch `release/v5.3` is recommended. For using the tag of ESP-IDF, the tag `v5.3.2` or later is recommended.
* Please follow the [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/get-started/index.html) to set up the development environment.

### Hardware Required

* Main board - Olimex [ESP32-P4-DevKit](https://www.olimex.com/Products/IoT/ESP32-P4/ESP32-P4-DevKit/open-source-hardware)
* Display - Olimex [MIPI-LCD2.8-640x480](https://www.olimex.com/Products/RaspberryPi/MIPI-LCD2.8-640x480/)
* Camera - Olimex [CAMERA-OV5647-5MPIX](https://www.olimex.com/Products/Components/Camera/CAMERA-OV5647-5MPIX/)
* USB cable - [USB-CABLE-AM-USB3-C]https://www.olimex.com/Products/Components/Cables/USB/USB-CABLE-AM-USB3-C/

Connect the camera to MIPI-CSI connector and connect the display to MIPI-DSI connector. Power the main board via the USB type C cable.

### Configurations

The demo is already configured, but if you wish you can check the menuconfig settigns the following way:

- Run `idf.py menuconfig`

  - See [Configuration Guide](../../../docs/envs/use_with_idf.md#configuration-guide) for more details.

### Build and Flash

Run `idf.py -p <PORT> build flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type `Ctrl-]`.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Troubleshooting

Please check the [FAQ](../../../docs/envs/use_with_idf.md#faq) first to see if the same question exists. If not, please create a [Github Issue](https://github.com/esp-arduino-libs/ESP32_Display_Panel/issues). We will get back to you as soon as possible.
