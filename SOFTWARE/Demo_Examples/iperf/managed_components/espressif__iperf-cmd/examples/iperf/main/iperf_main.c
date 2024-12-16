/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <errno.h>
#include <string.h>
#include <sys/socket.h>

/* Build check: LWIP_IPV4/LWIP_IPV6 should be defined after include sys/socket.h */
#if !(defined LWIP_IPV4) || !(defined LWIP_IPV6)
#error "Both LWIP_IPV4 and LWIP_IPV6 should be defined!"
#endif

#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "esp_console.h"

#include "iperf_cmd.h"
#include "wifi_cmd.h"

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    /* initialise wifi */
    app_initialise_wifi(NULL);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    repl_config.prompt = "iperf>";
    repl_config.max_history_len = 1;
    repl_config.task_priority = 24;
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&uart_config, &repl_config, &repl));

    /* Register commands */
    app_register_sta_basic_commands();
    ESP_ERROR_CHECK(app_register_iperf_commands());

    printf("\n ==================================================\n");
    printf(" |       Steps to test WiFi throughput            |\n");
    printf(" |                                                |\n");
    printf(" |  1. Print 'help' to gain overview of commands  |\n");
    printf(" |  2. Configure device to station or soft-AP     |\n");
    printf(" |  3. Setup WiFi connection                      |\n");
    printf(" |  4. Run iperf to test UDP/TCP RX/TX throughput |\n");
    printf(" |                                                |\n");
    printf(" =================================================\n\n");

    // start console REPL
    ESP_ERROR_CHECK(esp_console_start_repl(repl));
}
