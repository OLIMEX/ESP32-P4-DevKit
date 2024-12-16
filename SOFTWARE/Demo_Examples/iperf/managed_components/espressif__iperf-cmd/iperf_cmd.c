/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <sys/socket.h>
// #include "esp_idf_version.h"
#include "esp_log.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"
#include "esp_netif.h"

#include "iperf.h"


#ifndef APP_TAG
#define APP_TAG "IPERF"
#endif

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

typedef struct {
    struct arg_str *ip;
    struct arg_lit *server;
    struct arg_lit *udp;
#if IPERF_IPV6_ENABLED
    struct arg_lit *ipv6_domain;
#endif
    struct arg_int *port;
    struct arg_int *length;
    struct arg_int *interval;
    struct arg_int *time;
    struct arg_int *bw_limit;
    struct arg_str *format;
    struct arg_lit *abort;
    struct arg_end *end;
} iperf_args_t;
static iperf_args_t iperf_args;

static int cmd_do_iperf(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &iperf_args);
    iperf_cfg_t cfg;

    if (nerrors != 0) {
        arg_print_errors(stderr, iperf_args.end, argv[0]);
        return 0;
    }

    if (iperf_args.abort->count != 0) {
        iperf_stop();
        return 0;
    }

    if (g_iperf_is_running) {
        ESP_LOGE(APP_TAG, "iperf is already running.");
        return 1;
    }

    memset(&cfg, 0, sizeof(cfg));

    cfg.type = IPERF_IP_TYPE_IPV4;
#if IPERF_IPV6_ENABLED
    if (iperf_args.ipv6_domain->count > 0) {
        cfg.type = IPERF_IP_TYPE_IPV6;
    }
#endif

    if (((iperf_args.ip->count == 0) && (iperf_args.server->count == 0)) ||
            ((iperf_args.ip->count != 0) && (iperf_args.server->count != 0))) {
        ESP_LOGE(APP_TAG, "should specific client/server mode");
        return 0;
    }

    if (iperf_args.ip->count == 0) {
        cfg.flag |= IPERF_FLAG_SERVER;
    } else {
        cfg.flag |= IPERF_FLAG_CLIENT;
#if IPERF_IPV6_ENABLED
        if (cfg.type == IPERF_IP_TYPE_IPV6) {
            /* TODO: Refactor iperf config structure in v1.0 */
            cfg.destination_ip6 = (char*)(iperf_args.ip->sval[0]);
        }
#endif
#if IPERF_IPV4_ENABLED
        if (cfg.type == IPERF_IP_TYPE_IPV4) {
            cfg.destination_ip4 = esp_ip4addr_aton(iperf_args.ip->sval[0]);
        }
#endif
    }

    if (iperf_args.udp->count == 0) {
        cfg.flag |= IPERF_FLAG_TCP;
    } else {
        cfg.flag |= IPERF_FLAG_UDP;
    }

    if (iperf_args.length->count == 0) {
        cfg.len_send_buf = 0;
    } else {
        cfg.len_send_buf = iperf_args.length->ival[0];
    }

    if (iperf_args.port->count == 0) {
        cfg.sport = IPERF_DEFAULT_PORT;
        cfg.dport = IPERF_DEFAULT_PORT;
    } else {
        if (cfg.flag & IPERF_FLAG_SERVER) {
            cfg.sport = iperf_args.port->ival[0];
            cfg.dport = IPERF_DEFAULT_PORT;
        } else {
            cfg.sport = IPERF_DEFAULT_PORT;
            cfg.dport = iperf_args.port->ival[0];
        }
    }

    if (iperf_args.interval->count == 0) {
        cfg.interval = IPERF_DEFAULT_INTERVAL;
    } else {
        cfg.interval = iperf_args.interval->ival[0];
        if (cfg.interval <= 0) {
            cfg.interval = IPERF_DEFAULT_INTERVAL;
        }
    }

    if (iperf_args.time->count == 0) {
        cfg.time = IPERF_DEFAULT_TIME;
    } else {
        cfg.time = iperf_args.time->ival[0];
        if (cfg.time <= cfg.interval) {
            cfg.time = cfg.interval;
        }
    }
    /* iperf -b */
    if (iperf_args.bw_limit->count == 0) {
        cfg.bw_lim = IPERF_DEFAULT_NO_BW_LIMIT;
    } else {
        cfg.bw_lim = iperf_args.bw_limit->ival[0];
        if (cfg.bw_lim <= 0) {
            cfg.bw_lim = IPERF_DEFAULT_NO_BW_LIMIT;
        }
    }
    /* -f --format */
    cfg.format = MBITS_PER_SEC;
    if (iperf_args.format->count > 0) {
        char format_ch = iperf_args.format->sval[0][0];
        if (format_ch == 'k') {
            cfg.format = KBITS_PER_SEC;
        } else if (format_ch == 'm') {
            cfg.format = MBITS_PER_SEC;
        } else {
            ESP_LOGW(APP_TAG, "ignore invalid format: %c", format_ch);
        }
    }

    ESP_LOGI(APP_TAG, "mode=%s-%s sip=%s:%" PRId32 ", dip=%s:%" PRId32 ", interval=%" PRId32 ", time=%" PRId32,
             cfg.flag & IPERF_FLAG_TCP ? "tcp" : "udp",
             cfg.flag & IPERF_FLAG_SERVER ? "server" : "client",
             "localhost", cfg.sport,
#if IPERF_IPV4_ENABLED
             cfg.type == IPERF_IP_TYPE_IPV6? cfg.destination_ip6 : inet_ntoa(cfg.destination_ip4), cfg.dport,
#else
             cfg.type == IPERF_IP_TYPE_IPV6? cfg.destination_ip6 : "0.0.0.0", cfg.dport,
#endif
             cfg.interval, cfg.time);

    iperf_start(&cfg);

    return 0;
}

esp_err_t iperf_cmd_register_iperf(void)
{
    /* same with official iperf: https://iperf.fr/iperf-doc.php */
    iperf_args.ip = arg_str0("c", "client", "<host>", "run in client mode, connecting to <host>");
    iperf_args.server = arg_lit0("s", "server", "run in server mode");
    iperf_args.udp = arg_lit0("u", "udp", "use UDP rather than TCP");
#if IPERF_IPV6_ENABLED
    /*
     * NOTE: iperf2 uses -V(--ipv6_domain or --IPv6Version) for ipv6
     * iperf3 uses -6(--version6)
     * May add a new command "iperf3" in the future
     */
    iperf_args.ipv6_domain = arg_lit0("V", "ipv6_domain", "Set the domain to IPv6 (send packets over IPv6)");
#endif
    iperf_args.port = arg_int0("p", "port", "<port>", "server port to listen on/connect to");
    iperf_args.length = arg_int0("l", "len", "<length>", "length of buffer in bytes to write"
                                                         "(Defaults: TCP=" STR(IPERF_DEFAULT_TCP_TX_LEN)
                                                         ", IPv4 UDP=" STR(IPERF_DEFAULT_IPV4_UDP_TX_LEN)
                                                         ", IPv6 UDP=" STR(IPERF_DEFAULT_IPV6_UDP_TX_LEN) ")");
    iperf_args.interval = arg_int0("i", "interval", "<interval>", "seconds between periodic bandwidth reports");
    iperf_args.time = arg_int0("t", "time", "<time>", "time in seconds to transmit for (default 10 secs)");
    iperf_args.bw_limit = arg_int0("b", "bandwidth", "<bandwidth>", "bandwidth to send at in Mbits/sec");
    iperf_args.format = arg_str0("f", "format", "<format>", "'k' = Kbits/sec 'm' = Mbits/sec");
    /* abort is not an official option */
    iperf_args.abort = arg_lit0(NULL, "abort", "abort running iperf");
    iperf_args.end = arg_end(1);
    const esp_console_cmd_t iperf_cmd = {
        .command = "iperf",
        .help = "iperf command to measure network performance, through TCP or UDP connections.",
        .hint = NULL,
        .func = &cmd_do_iperf,
        .argtable = &iperf_args
    };

    return esp_console_cmd_register(&iperf_cmd);
}
