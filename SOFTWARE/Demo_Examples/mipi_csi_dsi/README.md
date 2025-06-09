| Supported ESP SoCs | ESP32-P4 |
| ------------------ | ----- |

# MIPI camera and display demo

## Overview

This example demonstrates how to stream camera input to the display. It requires camera, display, and ESP32-P4 board.

## How to use

### ESP-IDF Required

* This demo was made and tested with ESP-IDF `v5.4.1`.
* Please follow the [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/get-started/index.html) to set up the development environment.

### Hardware Required

* Main board - Olimex [ESP32-P4-DevKit](https://www.olimex.com/Products/IoT/ESP32-P4/ESP32-P4-DevKit/open-source-hardware)
* Display - Olimex [MIPI-LCD2.8-640x480](https://www.olimex.com/Products/RaspberryPi/MIPI-LCD2.8-640x480/)
* Camera - Olimex [CAMERA-OV5647-5MPIX](https://www.olimex.com/Products/Components/Camera/CAMERA-OV5647-5MPIX/)
* USB cable - Type C USB 3.0 cable [USB-CABLE-AM-USB3-C](https://www.olimex.com/Products/Components/Cables/USB/USB-CABLE-AM-USB3-C/)

Connect the camera to MIPI-CSI connector and connect the display to MIPI-DSI connector. Power the main board via the USB type C cable.

### Configurations

The demo is already configured, but if you wish you can check the menuconfig settigns the following way:

- Run `idf.py menuconfig`

  - See [Configuration Guide](../../../docs/envs/use_with_idf.md#configuration-guide) for more details.

### Build and Flash

Run `idf.py -p <PORT> build flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type `Ctrl-]`.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.
