/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_idf_version.h"

#define DEF_WIFI_CONN_MAX_RETRY_CNT (10)

typedef struct {
    bool reconnect;
    uint32_t reconnect_max_retry;
} wifi_cmd_config_t;

#define WIFI_CMD_CONFIG_DEFAULT() { \
    .reconnect = true, \
    .reconnect_max_retry = DEF_WIFI_CONN_MAX_RETRY_CNT, \
}

typedef struct {
    wifi_storage_t storage;
    wifi_ps_type_t ps_type;
} app_wifi_initialise_config_t;

#define APP_WIFI_CONFIG_DEFAULT() { \
    .storage = WIFI_STORAGE_FLASH, \
    .ps_type = WIFI_PS_MIN_MODEM, \
}

/**
 * Variables
 */
extern esp_netif_t *g_netif_ap;
extern esp_netif_t *g_netif_sta;
extern wifi_cmd_config_t g_wifi_cmd_config;
extern bool g_is_sta_wifi_connected;
extern bool g_is_sta_got_ip4;

/**
 * include: sta_connect, sta_disconnect
 */
void app_register_sta_basic_commands(void);

/**
  * Used in app_main for simple wifi test apps.
  */
void app_initialise_wifi(app_wifi_initialise_config_t *config);
