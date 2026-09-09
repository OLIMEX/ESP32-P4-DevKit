/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_h264_alloc.h"

void *esp_h264_aligned_calloc(uint32_t alignment, uint32_t n, uint32_t size, uint32_t *actual_size, uint32_t caps)
{
    *actual_size = ALIGN_UP(n * size, alignment);
    void *out_ptr = heap_caps_aligned_calloc((size_t)alignment, (size_t)n, (size_t)size, caps);
    return out_ptr;
}

void *esp_h264_calloc_prefer(uint32_t n, uint32_t size, uint32_t *actual_size, uint32_t caps1, uint32_t caps2)
{
    *actual_size = n * size;
    void *out_ptr = heap_caps_calloc_prefer((size_t)n, (size_t)size, 2, caps1, caps2);
    return out_ptr;
}
