/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
// #include "esp_idf_version.h"
#include "esp_log.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "wifi_cmd.h"

#define APP_TAG "iperf"
#ifndef MAC2STR
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#endif

#define LOG_WIFI_CMD_DONE(ret, desc) do { \
    if (ret == ESP_OK) { \
        ESP_LOGI(APP_TAG, "DONE.%s,OK.", desc); \
    } else { \
        ESP_LOGI(APP_TAG, "DONE.%s,FAIL.%d,%s", desc, ret, esp_err_to_name(ret)); \
    } \
} while(0)

#define WIFI_ERR_CHECK_LOG(ret, desc) do { \
    if (ret != ESP_OK) { \
        ESP_LOGW(APP_TAG, "@EW:failed:%s,%d,%s", desc, ret, esp_err_to_name(ret)); \
    } \
} while(0)

esp_netif_t *g_netif_ap = NULL;
esp_netif_t *g_netif_sta = NULL;
int g_wifi_connect_retry_cnt = 0;
bool g_is_sta_wifi_connected = false;
bool g_is_sta_got_ip4 = false;
wifi_cmd_config_t g_wifi_cmd_config = WIFI_CMD_CONFIG_DEFAULT();

void app_handler_on_sta_disconnected(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    g_is_sta_wifi_connected = false;
    g_is_sta_got_ip4 = false;
    wifi_event_sta_disconnected_t *event = (wifi_event_sta_disconnected_t *)event_data;
    ESP_LOGI(APP_TAG, "WIFI_EVENT_STA_DISCONNECTED! reason: %d", event->reason);

    if (!g_wifi_cmd_config.reconnect) {
        return;
    }
    g_wifi_connect_retry_cnt++;
    if (g_wifi_connect_retry_cnt > g_wifi_cmd_config.reconnect_max_retry) {
        return;
    }
    ESP_LOGI(APP_TAG, "trying to reconnect...");
    esp_err_t err = esp_wifi_connect();
    if (err != ESP_OK) {
        ESP_LOGE(APP_TAG, "WiFi Reconnect failed! (%s)", esp_err_to_name(err));
        return;
    }
}

void app_handler_on_sta_connected(void *esp_netif, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    g_is_sta_wifi_connected = true;
    ESP_LOGI(APP_TAG, "WIFI_EVENT_STA_CONNECTED!");
#if CONFIG_LWIP_IPV6
    esp_netif_create_ip6_linklocal(esp_netif);
#endif // CONFIG_LWIP_IPV6
}

void app_handler_on_sta_got_ip(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    g_is_sta_got_ip4 = true;
    g_wifi_connect_retry_cnt = 0;
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    ESP_LOGI(APP_TAG, "IP_EVENT_STA_GOT_IP: Interface \"%s\" address: " IPSTR, esp_netif_get_desc(event->esp_netif), IP2STR(&event->ip_info.ip));
    ESP_LOGI(APP_TAG, "- IPv4 address: " IPSTR ",", IP2STR(&event->ip_info.ip));

}

#if CONFIG_LWIP_IPV6
void app_handler_on_sta_got_ipv6(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    ip_event_got_ip6_t *event = (ip_event_got_ip6_t *)event_data;
    esp_ip6_addr_type_t ipv6_type = esp_netif_ip6_get_addr_type(&event->ip6_info.ip);
    ESP_LOGI(APP_TAG, "IP_EVENT_GOT_IP6: Interface \"%s\" address: " IPV6STR ", type: %d", esp_netif_get_desc(event->esp_netif),
             IPV62STR(event->ip6_info.ip), ipv6_type);
    ESP_LOGI(APP_TAG, "- IPv6 address: " IPV6STR ", type: %d", IPV62STR(event->ip6_info.ip), ipv6_type);
}
#endif // CONFIG_LWIP_IPV6

void app_handler_on_scan_done(void *arg, esp_event_base_t event_base,
                              int32_t event_id, void *event_data)
{
    uint16_t sta_number = 0;
    uint8_t i;
    wifi_ap_record_t *ap_list_buffer;

    esp_wifi_scan_get_ap_num(&sta_number);
    if (!sta_number) {
        ESP_LOGE(APP_TAG, "SCAN_DONE: No AP found");
        return;
    }

    ap_list_buffer = malloc(sta_number * sizeof(wifi_ap_record_t));
    if (ap_list_buffer == NULL) {
        ESP_LOGE(APP_TAG, "SCAN_DONE: Failed to malloc buffer to print scan results");
        esp_wifi_clear_ap_list(); /* IDF >= v4.3 */
        return;
    }

    if (esp_wifi_scan_get_ap_records(&sta_number, (wifi_ap_record_t *)ap_list_buffer) == ESP_OK) {
        for (i = 0; i < sta_number; i++) {

            char phy_str[10] = "";
#if CONFIG_SOC_WIFI_HE_SUPPORT
            if (ap_list_buffer[i].phy_11ax) {
                sprintf(phy_str, "11ax");
            } else
#endif
                if (ap_list_buffer[i].phy_11n) {
                    sprintf(phy_str, "11n");
                } else if (ap_list_buffer[i].phy_11g) {
                    sprintf(phy_str, "11g");
                } else if (ap_list_buffer[i].phy_11b) {
                    sprintf(phy_str, "11b");
                }

            ESP_LOGI(APP_TAG,
                     "+SCAN:["MACSTR"][%s][rssi=%d][authmode=%d][ch=%d][second=%d][%s]"
#if CONFIG_SOC_WIFI_HE_SUPPORT
                     "[bssid-index=%d][bss_color=%d][disabled=%d]"
#endif
                     , MAC2STR(ap_list_buffer[i].bssid), ap_list_buffer[i].ssid, ap_list_buffer[i].rssi, ap_list_buffer[i].authmode,
                     ap_list_buffer[i].primary, ap_list_buffer[i].second, phy_str
#if CONFIG_SOC_WIFI_HE_SUPPORT
                     , ap_list_buffer[i].he_ap.bssid_index, ap_list_buffer[i].he_ap.bss_color, ap_list_buffer[i].he_ap.bss_color_disabled
#endif
                    );
        }
    }
    free(ap_list_buffer);
    ESP_LOGI(APP_TAG, "SCAN_DONE: Found %d APs", sta_number);
}

/**
  * Used in app_main for simple wifi test apps.
  */
void app_initialise_wifi(app_wifi_initialise_config_t *config)
{
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    g_netif_ap = esp_netif_create_default_wifi_ap();
    assert(g_netif_ap);
    g_netif_sta = esp_netif_create_default_wifi_sta();
    assert(g_netif_sta);

    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &app_handler_on_sta_disconnected, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_CONNECTED, &app_handler_on_sta_connected, g_netif_sta));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_SCAN_DONE, &app_handler_on_scan_done, g_netif_sta));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &app_handler_on_sta_got_ip, NULL));
#if CONFIG_LWIP_IPV6
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_GOT_IP6, &app_handler_on_sta_got_ipv6, NULL));
#endif

    if (config != NULL) {
        ESP_ERROR_CHECK(esp_wifi_set_storage(config->storage));
        ESP_ERROR_CHECK(esp_wifi_set_ps(config->ps_type));
    }
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
}

typedef struct {
    struct arg_str *ssid;
    struct arg_str *password;
    struct arg_int *channel;
    struct arg_end *end;
} sta_connect_args_t;
static sta_connect_args_t connect_args;

typedef struct {
    struct arg_str *ssid;
    struct arg_int *channel;
    struct arg_end *end;
} sta_scan_args_t;
static sta_scan_args_t scan_args;

static int cmd_do_sta_connect(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &connect_args);

    if (nerrors != 0) {
        arg_print_errors(stderr, connect_args.end, argv[0]);
        return 1;
    }

    wifi_config_t wifi_config = {
        .sta = {
            .scan_method = WIFI_ALL_CHANNEL_SCAN,
            .sort_method = WIFI_CONNECT_AP_BY_SIGNAL,
        },
    };

    const char *ssid = connect_args.ssid->sval[0];
    memcpy((char *) wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid));
    const char *pass = connect_args.password->sval[0];
    if (connect_args.password->count > 0) {
        memcpy((char *) wifi_config.sta.password, pass, sizeof(wifi_config.sta.password));
        wifi_config.sta.threshold.authmode = WIFI_AUTH_WEP;
    }
    if (connect_args.channel->count > 0) {
        wifi_config.sta.channel = (uint8_t)(connect_args.channel->ival[0]);
    }
    g_wifi_connect_retry_cnt = 0;

    ESP_LOGI(APP_TAG, "Connecting to %s...", ssid);
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    esp_err_t err = esp_wifi_connect();
    LOG_WIFI_CMD_DONE(err, "WIFI_CONNECT_START");
    return 0;
}

static void app_register_sta_connect(void)
{
    connect_args.ssid = arg_str1(NULL, NULL, "<ssid>", "SSID of AP");
    connect_args.password = arg_str0(NULL, NULL, "<pass>", "password of AP");
    connect_args.channel = arg_int0("n", "channel", "<channel>", "channel of AP");
    connect_args.end = arg_end(2);
    const esp_console_cmd_t connect_cmd = {
        .command = "sta_connect",
        .help = "WiFi is station mode, join specified soft-AP",
        .hint = NULL,
        .func = &cmd_do_sta_connect,
        .argtable = &connect_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&connect_cmd));
}

static int cmd_do_sta_disconnect(int argc, char **argv)
{
    g_wifi_cmd_config.reconnect = false;
    esp_err_t err = esp_wifi_disconnect();
    LOG_WIFI_CMD_DONE(err, "WIFI_DISCONNECT");
    return 0;
}

static void app_register_sta_disconnect(void)
{
    const esp_console_cmd_t wifi_connect_cmd = {
        .command = "sta_disconnect",
        .help = "WiFi is station mode, stop wifi connect.",
        .hint = NULL,
        .func = &cmd_do_sta_disconnect,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&wifi_connect_cmd));
}

static int cmd_do_sta_scan(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &scan_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, scan_args.end, argv[0]);
        return 1;
    }
    uint8_t ssid[33] = {0};

    wifi_scan_config_t scan_config = {0};
    if (scan_args.ssid->count > 0) {
        strlcpy((char *)ssid, scan_args.ssid->sval[0], 33);
        scan_config.ssid = ssid;
    }
    if (scan_args.channel->count > 0) {
        scan_config.channel = (uint8_t)(scan_args.channel->ival[0]);
    }

    esp_err_t err = esp_wifi_scan_start(&scan_config, false);
    LOG_WIFI_CMD_DONE(err, "STA_SCAN_START");
    return 0;
}

static int cmd_do_wifi_scan(int argc, char **argv)
{
    ESP_LOGW(APP_TAG, "DEPRECATED, please use sta_scan.");
    return cmd_do_sta_scan(argc, argv);
}

static void app_register_sta_scan(void)
{
    scan_args.ssid = arg_str0(NULL, NULL, "<ssid>", "SSID of AP");
    scan_args.channel = arg_int0("n", "channel", "<channel>", "channel of AP");
    scan_args.end = arg_end(2);
    const esp_console_cmd_t scan_cmd = {
        .command = "wifi_scan",
        .help = "WiFi is station mode, Scan APs",
        .hint = NULL,
        .func = &cmd_do_wifi_scan,
        .argtable = &scan_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&scan_cmd));

    const esp_console_cmd_t scan_cmd_alias = {
        .command = "sta_scan",
        .help = "WiFi is station mode, Scan APs",
        .hint = NULL,
        .func = &cmd_do_sta_scan,
        .argtable = &scan_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&scan_cmd_alias));
}

void app_register_sta_basic_commands(void)
{
    app_register_sta_connect();
    app_register_sta_disconnect();
    app_register_sta_scan();
}
