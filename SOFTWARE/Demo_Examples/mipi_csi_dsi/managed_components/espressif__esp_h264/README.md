# ESP_H264
- [![Component Registry](https://components.espressif.com/components/espressif/esp_h264/badge.svg)](https://components.espressif.com/components/espressif/esp_h264)

ESP_H264 is Espressif's lightweight H.264 encoder and decoder component, offering both hardware and software implementations. The hardware encoder (HW encoder) is designed specifically for the esp32p4 chip, achieving a frame rate greater than 30fps for 1080P resolution images. The software encoder (SW encoder) is sourced from v2.2.0 of [openh264](https://github.com/cisco/openh264), while the decoder is obtained from [tinyH264](https://github.com/udevbe/tinyh264). Both the software encoder and decoder are optimized for memory and CPU usage, ensuring optimal performance on Espressif chips.

# Term

| Term Abbr  | Full Name               | Description                                                                             |
| ---------- | ----------------------- | --------------------------------------------------------------------------------------- |
| QP         | Quantization parameter  | The higher the QP, the higher the compression rate and the worse the image quality.     |
|            |                         | QP is within the range of 0 to 51.                                                      |
| FPS        | Frames per second       | It is related to the smoothness of the video. It can be set to 24 in the general video. |
| I-frame    | Intra frame             | Frames that can be encoded without reference to other frames.                           |
| IDR-frame  | Instantaneous decoding  | Special I frame.                                                                        |
|            | refresh frame           | Decoder can start decoding at this frame.                                               |
| P-frame    | Predicted frame         | Frames that must be encoded decoded refering to other frames.                           |
| GOP        | Group of picture        | The sum of one I-frame and number of pictures between two I frames.                     |
|            |                         | GOP is usually set to the number of FPS output by the encoder.                          |
| Resolution | Resolution              | It means the width and height of picture.                                               |
| MB         | Marco block             | For picture luma, MB size is 16x16. And for picture chrominance,  MB size is 8x8.       |
|            |                         | `mb_width` = (`width` + 15) >> 4. `mb_height` = (`height` + 15) >> 4                    |
| Slice      | Slice                   | Multiple macroblocks form a slice                                                       |
| MV         | Motion vector           | The horizontal and vertical displacement of the current MB relative                     |
|            |                         | to the best matching MB in the previous frame.                                          |
| ROI        | Range of interest       | The area of ​​interest is in a picture.                                                   |
|            |                         | This area can be set with different QP to make it clearer or blurrier.                  |
|            |                         | The quantization unit is the size of a luma MB.                                         |
| SPS        | Sequence parameter set  | Required to start decoding.                                                             |
| PPS        | Picture parameter set   | Required to start decoding.                                                             |
| PTS        | Presentation time stamp | In baseline profile, the PTS is same as DTS.                                            |
| DTS        | Decoding time stamp     | In baseline profile, the DTS is same as PTS.                                            |
| RC         | Rate control            | The size of the output stream approaching the target stream is controlled.              |
|            |                         |                                                                                         |

# Supported chip

| ESP_H264 Version | ESP32-S3  | ESP32-P4  |
| ---------------- | --------- | --------- |
| v1.0.x           | Supported | Supported |

# Features

## encoder
| feature             | HW encoder                                                          | SW encoder                                  |
| ------------------- | ------------------------------------------------------------------- | ------------------------------------------- |
| profile             | Support baseline profile                                            | Support baseline profile                    |
| width               | Supported range is 80 to 1088.                                      | Supported range is gather than or equal 16. |
| height              | Supported range is 80 to 2048.                                      | Supported range is gather than or equal 16. |
| QP                  | Supported all                                                       | Supported all                               |
| FPS                 | Supported FPS range is 1 to 255.                                    | Supported FPS range is 1 to 255.            |
| GOP                 | Supported GOP range is 1 to 255.                                    | Supported GOP range is 1 to 255.            |
| SPS                 | Supported SPS is for all IDR-frame                                  | Supported SPS is for all IDR-frame          |
| PPS                 | Supported SPS is for all IDR-frame                                  | Supported SPS is for all IDR-frame          |
| unencoded data type | Supported ESP_H264_RAW_FMT_O_UYY_E_VYY                              | Supported ESP_H264_RAW_FMT_YUYV             |
|                     |                                                                     | Supported ESP_H264_RAW_FMT_I420             |
| RC                  | Supported                                                           | Supported                                   |
| de-blocking filter  | Supported                                                           | Supported                                   |
| Single stream       | Supported                                                           | Supported                                   |
| Dual stream         | Each stream supports different parameter configurations except GOP. | Un-supported                                |
| ROI                 | Supported ROI region number isn't gahter than 8.                    | Un-supported                                |
|                     | Each region supports fixed QP or delta QP.                          | Un-supported                                |
|                     | Each none region supports delta QP.                                 | Un-supported                                |
| MV                  | Supported output MV data                                            | Un-supported                                |
|                     |                                                                     | Un-supported                                |
## decoder
| feature                                    |  SW encoder                                  |
| -------------------                        |  ------------------------------------------- |
| profile                                    |  Support baseline profile                    |
| width                                      |  Supported range is gather than or equal 16. |
| height                                     |  Supported range is gather than or equal 16. |
| QP                                         |  Supported all                               |
| FPS                                        |  Supported                                   |
| GOP                                        |  Supported                                   |
| SPS                                        |  Supported                                   |
| PPS                                        |  Supported                                   |
| unencoded data type                        |  Supported ESP_H264_RAW_FMT_I420             |
| long term reference (LTR) frames           |  Supported                                   |
| memory management control operation (MMCO) |  Supported                                   |
| reference picture list modification        |  Supported                                   |

# Performance

## Test on chip ESP32-S3R8

### SW ENCODER

| Resolution | Raw Format              | Memory (Byte) | Frame Per Second(fps) |
| ---------- | ----------------------- | ------------- | --------------------- |
| 320 * 192  | ESP_H264_RAW_FMT_I420   | 1 M           | 17.48                 |
| 320 * 240  | ESP_H264_RAW_FMT_YUYV   | 1 M           | 11.23                 |

### DECODER

Note: the memory consumption is strongly depenedent on the resolution of H264 stream. and the encoder data.

| Resolution |     Raw Format          | Memory (Byte) | Frame Per Second(fps) |
| --         | --                      |--             | --                    |
| 640 * 360  | ESP_H264_RAW_FMT_I420   |6.2 M          | 7.38                  |
| 320 * 192  | ESP_H264_RAW_FMT_I420   |1.0 M          | 18.87                 |

## Test on chip ESP32-P4

### HW ENCODER

| Resolution  | Raw Format                    | Memory (Byte) | Frame Per Second(fps) |
| ----------- | ----------------------------- | ------------- | --------------------- |
| 1920 * 1080 | ESP_H264_RAW_FMT_O_UYY_E_VYY  | 140k          | 30                    |

### DECODER

Note: the memory consumption is strongly depenedent on the resolution of H264 stream. and the encoder data.

| Resolution |     Raw Format          | Memory (Byte) | Frame Per Second(fps) |
| --         | --                      |--             | --                    |
| 1280 * 720 | ESP_H264_RAW_FMT_I420   |6.2 M          | 4                     |

# Example
Please refer to the files test_apps/esp_h264_*_test.c and test_apps/esp_h264_*_test.h for more details on API usage.
