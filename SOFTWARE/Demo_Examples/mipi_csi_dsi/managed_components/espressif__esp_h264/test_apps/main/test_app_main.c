/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "unity.h"
#include "unity_test_utils.h"
#include "esp_heap_caps.h"
#include "sdkconfig.h"

#define TEST_MEMORY_LEAK_THRESHOLD (300)

void setUp(void)
{
    unity_utils_record_free_mem();
}

void tearDown(void)
{
    unity_utils_evaluate_leaks_direct(TEST_MEMORY_LEAK_THRESHOLD);
}

void app_main(void)
{
    /*
     _____         _     _      _   _______   _______   —     —
    |_   _|       | |   | |    | | |_____  | |  _____| | |   | |
      | | ___  ___| |_  | |____| |  _____| | | |_____  | |___| |_
      | |/ _ \/ __| __| |  ____  | |  _____| |  ___  | |_____   _|
      | |  __/\__ \ |_  | |    | | | |_____  | |___| |       | |
      \_/\___||___/\__| |_|    |_| |_______| |_______|       |_|
    */

    printf(" _____         _     _      _   _______   _______   —     — \n");
    printf("|_   _|       | |   | |    | | |_____  | |  _____| | |   | | \n");
    printf("  | | ___  ___| |_  | |____| |  _____| | | |_____  | |___| |_ \n");
    printf("  | |/ _ \\/ __| __| |  ____  | |  _____| |  ___  | |_____   _| \n");
    printf("  | |  __/\\__ \\ |_  | |    | | | |_____  | |___| |       | | \n");
    printf("  \\_/\\___||___/\\__| |_|    |_| |_______| |_______|       |_| \n");

    unity_run_menu();
}
