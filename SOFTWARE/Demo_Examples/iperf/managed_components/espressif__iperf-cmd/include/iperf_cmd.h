/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Registers console commands: iperf.
 *
 * @return ESP_OK on success
 */
esp_err_t iperf_cmd_register_iperf(void);

/* TODO: deprecate app_xxx in v0.2, remove them in v1.0 */
#define app_register_iperf_commands iperf_cmd_register_iperf

#ifdef __cplusplus
}
#endif
