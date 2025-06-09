#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/param.h>
#include <sys/errno.h>

#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"

#include <fcntl.h>

#include "esp_check.h"

#include "esp_display_panel.hpp"
#include "esp_lib_utils.h"
#include "esp_heap_caps.h"
#include "esp_mac.h"

#include "linux/videodev2.h"
#include "esp_video_device.h"
#include "esp_video_init.h"

#include "driver/ppa.h"

using namespace esp_panel::drivers;
using namespace esp_panel::board;

static const char *TAG = "example";

#define FRAME_BUFFER_COUNT           (2)
#define CAMERA_HRES                  (800) 
#define CAMERA_VRES                  (640)
#define CAMERA_COLOR_FORMAT          V4L2_PIX_FMT_RGB24
#define CAMERA_BUFFER_MEMORY_TYPE    V4L2_MEMORY_USERPTR
#define CAMERA_BUFFER_MEMORY_ALIGN   (64)

esp_err_t ppa_init(ppa_client_handle_t *ppa_srm_handle) {
    ppa_client_config_t ppa_srm_config = {
        .oper_type = PPA_OPERATION_SRM,
        .max_pending_trans_num = 1,
    };
    ESP_RETURN_ON_ERROR(ppa_register_client(&ppa_srm_config, ppa_srm_handle), TAG, "PPA SRM Init FAIL");
    return ESP_OK;
}

static esp_err_t camera_capabilities(int mipi_device) {
    struct v4l2_capability capability;

    if (ioctl(mipi_device, VIDIOC_QUERYCAP, &capability) != 0) {
        return ESP_FAIL;
    }

    ESP_LOGI(
        TAG, "Version: %d.%d.%d", 
        (uint16_t)(capability.version >> 16),
        (uint8_t)(capability.version >> 8),
        (uint8_t)capability.version
    );
    ESP_LOGI(TAG, "Driver:  %s", capability.driver);
    ESP_LOGI(TAG, "Card:    %s", capability.card);
    ESP_LOGI(TAG, "Bus:     %s", capability.bus_info);
    ESP_LOGI(TAG, "Capabilities:");
    if (capability.capabilities & V4L2_CAP_VIDEO_CAPTURE) {
        ESP_LOGI(TAG, "\tVIDEO_CAPTURE");
    }
    if (capability.capabilities & V4L2_CAP_READWRITE) {
        ESP_LOGI(TAG, "\tREADWRITE");
    }
    if (capability.capabilities & V4L2_CAP_ASYNCIO) {
        ESP_LOGI(TAG, "\tASYNCIO");
    }
    if (capability.capabilities & V4L2_CAP_STREAMING) {
        ESP_LOGI(TAG, "\tSTREAMING");
    }
    if (capability.capabilities & V4L2_CAP_META_OUTPUT) {
        ESP_LOGI(TAG, "\tMETA_OUTPUT");
    }
    if (capability.capabilities & V4L2_CAP_DEVICE_CAPS) {
        ESP_LOGI(TAG, "Device capabilities:");
        if (capability.device_caps & V4L2_CAP_VIDEO_CAPTURE) {
            ESP_LOGI(TAG, "\tVIDEO_CAPTURE");
        }
        if (capability.device_caps & V4L2_CAP_READWRITE) {
            ESP_LOGI(TAG, "\tREADWRITE");
        }
        if (capability.device_caps & V4L2_CAP_ASYNCIO) {
            ESP_LOGI(TAG, "\tASYNCIO");
        }
        if (capability.device_caps & V4L2_CAP_STREAMING) {
            ESP_LOGI(TAG, "\tSTREAMING");
        }
        if (capability.device_caps & V4L2_CAP_META_OUTPUT) {
            ESP_LOGI(TAG, "\tMETA_OUTPUT");
        }
    }

    return ESP_OK;
}

static esp_err_t camera_get_format(int mipi_device, struct v4l2_format *format) {
    memset(format, 0, sizeof(struct v4l2_format));
    
    format->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(mipi_device, VIDIOC_G_FMT, format) != 0) {
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Format: %lu x %lu (0x%04lX)", format->fmt.pix.width, format->fmt.pix.height, format->fmt.pix.pixelformat);

    return ESP_OK;
}

static esp_err_t camera_enumerate_formats(int mipi_device) {
    struct v4l2_format format;
    uint32_t fmt_index = 1;

    ESP_RETURN_ON_ERROR(camera_get_format(mipi_device, &format), TAG, "Failed to get camera format");

    while (1) {
        struct v4l2_fmtdesc fmtdesc = {
            .index = fmt_index,
            .type = V4L2_BUF_TYPE_VIDEO_CAPTURE,
        };

        if (ioctl(mipi_device, VIDIOC_ENUM_FMT, &fmtdesc) != 0) {
            if (fmt_index == 1) {
                return ESP_FAIL;
            }
            return ESP_OK;
        }

        ESP_LOGI(TAG, "\t[%lu] %s (0x%04lX)", fmt_index, (char *)fmtdesc.description, fmtdesc.pixelformat);
        fmt_index++;
    }

    return ESP_OK;
}

static esp_err_t camera_set_format(int mipi_device, uint32_t width, uint32_t height, uint32_t pixelformat) {
    struct v4l2_format format = {
        .type = V4L2_BUF_TYPE_VIDEO_CAPTURE,
        .fmt = {
            .pix = {
                .width = width,
                .height = height,
                .pixelformat = pixelformat,
            }
        },
    };

    if (ioctl(mipi_device, VIDIOC_S_FMT, &format) != 0) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

static esp_err_t camera_begin(int *mipi_device, uint32_t width, uint32_t height, uint32_t pixelformat, bool verbose) {
    *mipi_device = open(ESP_VIDEO_MIPI_CSI_DEVICE_NAME, O_RDONLY);
    if (*mipi_device < 0) {
        ESP_LOGE(TAG, "Failed to open MIPI device");
        return ESP_FAIL;
    }

    if (verbose) {
        ESP_RETURN_ON_ERROR(camera_capabilities(*mipi_device), TAG, "Failed to get camera capabilities");
        ESP_RETURN_ON_ERROR(camera_enumerate_formats(*mipi_device), TAG, "Failed to enumerate camera formats");
    }

    ESP_RETURN_ON_ERROR(camera_set_format(*mipi_device, width, height, pixelformat), TAG, "Failed to set camera format");

    return ESP_OK;
}

static esp_err_t camera_end(int mipi_device) {
    close(mipi_device);
    return ESP_OK;
}

static esp_err_t camera_request_buffers(int mipi_device, uint32_t count, uint32_t type) {
    struct v4l2_requestbuffers request;
    memset(&request, 0, sizeof(struct v4l2_requestbuffers));
    request.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    request.count  = count;
    request.memory = type;
    if (ioctl(mipi_device, VIDIOC_REQBUFS, &request) != 0) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

static esp_err_t camera_allocate_buffers(int mipi_device, uint32_t count, uint32_t type, uint8_t *buffer[], uint32_t buffer_size[]) {
    for (int i=0; i<count; i++) {
        struct v4l2_buffer buf;

        memset(&buf, 0, sizeof(buf));
        buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = type;
        buf.index  = i;
        if (ioctl(mipi_device, VIDIOC_QUERYBUF, &buf) != 0) {
            ESP_LOGE(TAG, "Failed to query buffer %d", i);
            return ESP_FAIL;
        }

        buffer[i] = (uint8_t*)heap_caps_aligned_alloc(CAMERA_BUFFER_MEMORY_ALIGN, buf.length, MALLOC_CAP_SPIRAM | MALLOC_CAP_CACHE_ALIGNED);
        if (!buffer[i]) {
            ESP_LOGE(TAG, "Failed to map buffer %d", i);
            return ESP_FAIL;
        }

        buf.m.userptr = (unsigned long)buffer[i];
        buffer_size[i] = buf.length;

        if (ioctl(mipi_device, VIDIOC_QBUF, &buf) != 0) {
            ESP_LOGE(TAG, "Failed to queue video buffer %d", i);
            return ESP_FAIL;
        }
    }
    return ESP_OK;
}

static esp_err_t camera_free_buffers(int mipi_device, uint32_t count, uint8_t *buffer[]) {
    for (int i=0; i<count; i++) {
        heap_caps_free(buffer[i]);
    }
    return ESP_OK;
}

static esp_err_t camera_start_stream(int mipi_device) {
    const int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(mipi_device, VIDIOC_STREAMON, &type) != 0) {
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Start stream");
    return ESP_OK;
}

static esp_err_t camera_stop_stream(int mipi_device) {
    const int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(mipi_device, VIDIOC_STREAMOFF, &type) != 0) {
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Stop stream");
    return ESP_OK;
}

static esp_err_t camera_capture_frame(int mipi_device, uint32_t type, uint8_t *buffer[], uint32_t buffer_size[], uint32_t *index) {
    struct v4l2_buffer buf;
    memset(&buf, 0, sizeof(buf));
    buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = type;
    if (ioctl(mipi_device, VIDIOC_DQBUF, &buf) != 0) {
        ESP_LOGE(TAG, "Failed to receive video frame");
        return ESP_FAIL;
    }

    buf.m.userptr = (unsigned long)buffer[buf.index];
    buf.length = buffer_size[buf.index];

    if (ioctl(mipi_device, VIDIOC_QBUF, &buf) != 0) {
        ESP_LOGE(TAG, "Failed to queue video frame");
        return ESP_FAIL;
    }

    *index = buf.index;
    return ESP_OK;
}

static esp_err_t camera_capture_stream(int mipi_device, LCD *lcd) {
    uint8_t *cam_buffer[FRAME_BUFFER_COUNT];
    uint32_t cam_buffer_size[FRAME_BUFFER_COUNT];

    // Prepare camera buffers
    struct v4l2_format camera_format;
    ESP_RETURN_ON_ERROR(camera_get_format(mipi_device, &camera_format), TAG, "Failed to get camera format");
    uint32_t cam_width = camera_format.fmt.pix.width;
    uint32_t cam_height = camera_format.fmt.pix.height;

    ESP_RETURN_ON_ERROR(camera_request_buffers(mipi_device, FRAME_BUFFER_COUNT, CAMERA_BUFFER_MEMORY_TYPE), TAG, "Failed to require buffers");
    ESP_RETURN_ON_ERROR(camera_allocate_buffers(mipi_device, FRAME_BUFFER_COUNT, CAMERA_BUFFER_MEMORY_TYPE, cam_buffer, cam_buffer_size), TAG, "Failed to allocate buffers");
    ESP_RETURN_ON_ERROR(camera_start_stream(mipi_device), TAG, "Failed to start stream");

    // Prepare LCD buffers
    uint32_t lcd_width = lcd->getFrameWidth();
    uint32_t lcd_height = lcd->getFrameHeight();
    ESP_LOGI(TAG, "LCD %lu x %lu", lcd_width, lcd_height);

    size_t lcd_buffer_size = lcd_width * lcd_height * 3;
    uint8_t *lcd_buffer[FRAME_BUFFER_COUNT];
    for (uint8_t i=0; i<FRAME_BUFFER_COUNT; i++) {
        lcd_buffer[i] = (uint8_t *)lcd->getFrameBufferByIndex(i);
    }

    // Init transformation
    ppa_client_handle_t ppa_srm_handle;
    ESP_RETURN_ON_ERROR(ppa_init(&ppa_srm_handle), TAG, "Failed to init PPA");

    ppa_srm_oper_config_t srm_config = {
        .in = {
            .buffer = nullptr,
            .pic_w = cam_width,
            .pic_h = cam_height,
            .block_w = lcd_height,
            .block_h = lcd_width,
            .block_offset_x = (cam_width  - lcd_height) / 2,
            .block_offset_y = (cam_height - lcd_width) / 2,
            .srm_cm = PPA_SRM_COLOR_MODE_RGB888,
        },
        .out = {
            .buffer = nullptr,
            .buffer_size = lcd_buffer_size,
            .pic_w = lcd_width,
            .pic_h = lcd_height,
            .block_offset_x = 0,
            .block_offset_y = 0,
            .srm_cm = PPA_SRM_COLOR_MODE_RGB888,
        },
        .rotation_angle = PPA_SRM_ROTATION_ANGLE_90,
        .scale_x = 1,
        .scale_y = 1,
        .rgb_swap = 0,
        .byte_swap = 0,
        .mode = PPA_TRANS_MODE_BLOCKING,
    };

    while (1) {
        uint32_t cur_buff_index;
        ESP_RETURN_ON_ERROR(camera_capture_frame(mipi_device, CAMERA_BUFFER_MEMORY_TYPE, cam_buffer, cam_buffer_size, &cur_buff_index), TAG, "Failed to capture frame");
        
        // Set proper input and output buffer
        srm_config.in.buffer = cam_buffer[cur_buff_index];
        srm_config.out.buffer = lcd_buffer[cur_buff_index];
        
        // Do transformation
        ESP_RETURN_ON_ERROR(ppa_do_scale_rotate_mirror(ppa_srm_handle, &srm_config), TAG, "Failed to transform frame");
        lcd->switchFrameBufferTo(lcd_buffer[cur_buff_index]);

        vTaskDelay(pdMS_TO_TICKS(1));
    }

    ESP_RETURN_ON_ERROR(camera_stop_stream(mipi_device), TAG, "Failed to stop stream");
    ESP_RETURN_ON_ERROR(camera_free_buffers(mipi_device, FRAME_BUFFER_COUNT, cam_buffer), TAG, "Failed to free buffers");

    return ESP_OK;
}

static esp_err_t camera_init(void) {
    static esp_video_init_csi_config_t csi_config[] = {
        {
            .sccb_config = {
                .init_sccb = true,
                .i2c_config = {
                    .port    = CONFIG_EXAMPLE_MIPI_I2C_PORT + 1, // FIX ME
                    .scl_pin = CONFIG_EXAMPLE_MIPI_I2C_SCL_PIN,
                    .sda_pin = CONFIG_EXAMPLE_MIPI_I2C_SDA_PIN,
                },
                .freq = CONFIG_EXAMPLE_MIPI_I2C_FREQ,
            },
            .reset_pin = CONFIG_EXAMPLE_MIPI_CSI_CAM_SENSOR_RESET_PIN,
            .pwdn_pin  = CONFIG_EXAMPLE_MIPI_CSI_CAM_SENSOR_PWDN_PIN,
        },
    };

    static const esp_video_init_config_t cam_config = {
        .csi = csi_config
    };

    esp_err_t ret = esp_video_init(&cam_config);
    if (ret != ESP_OK) {
        return ret;
    }

    return ESP_OK;
}

extern "C" void app_main(void) {
    Board *board = new Board();
    ESP_RETURN_VOID_ON_FALSE(board, ESP_ERR_INVALID_RESPONSE, TAG, "new Board failed");

    // See BOARD_OLIMEX_ESP32_P4_DEVKIT.h
    ESP_LOGI(TAG, "Initializing board");
    ESP_RETURN_VOID_ON_FALSE(board->init(), ESP_ERR_INVALID_RESPONSE, TAG, "Board init failed");
    ESP_RETURN_VOID_ON_FALSE(board->getLCD()->configFrameBufferNumber(FRAME_BUFFER_COUNT), ESP_ERR_INVALID_RESPONSE, TAG, "Failed to set LCD buffers");
    ESP_RETURN_VOID_ON_FALSE(board->begin(), ESP_ERR_INVALID_RESPONSE, TAG, "Board begin failed");
    
    // OV5647
    // Format: 800 x 640 (0x50424752)
    //   [1] RGB 5-6-5 (0x50424752)
    //   [2] RGB 8-8-8 (0x33424752)
    //   [3] YUV 4:2:0 (0x32315559)
    //   [4] YVU 4:2:2 planar (0x50323234)
    //   [5] RAW8 BGGR (0x31384142)
    int cam_mipi_dev;
    ESP_LOGI(TAG, "Initializing camera"); 
    ESP_RETURN_VOID_ON_ERROR(camera_init(), TAG, "Camera init failed");
    ESP_RETURN_VOID_ON_ERROR(camera_begin(&cam_mipi_dev, CAMERA_HRES, CAMERA_VRES, CAMERA_COLOR_FORMAT, true), TAG, "Failed to start camera");
    ESP_RETURN_VOID_ON_ERROR(camera_capture_stream(cam_mipi_dev, board->getLCD()), TAG, "Camera capture stream failed");
    ESP_RETURN_VOID_ON_ERROR(camera_end(cam_mipi_dev), TAG, "Failed to stop camera");
}