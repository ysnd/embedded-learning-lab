#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "hal/gpio_types.h"
#include "nvs_flash.h"
#include "esp_http_server.h"
#include "driver/gpio.h"

#define LED 2
//handler ws
static esp_err_t ws_handler(httpd_req_t *req) {
    if (req->method == HTTP_GET) {
        printf("Handshake done, new connection opened\n");
        return ESP_OK;
    }
    httpd_ws_frame_t ws_pkt;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;

    //get metadata frame    
    esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, 0);
    if (ret !=ESP_OK) {
        printf("Failed to get frane length:%s\n", esp_err_to_name(ret));
        return ret;
    }

    uint8_t *buf = calloc(1, ws_pkt.len + 1);
    if (!buf) {
        printf("Failed to alloc memory\n");
        return ESP_ERR_NO_MEM;
    }
    ws_pkt.payload = buf;
    ret = httpd_ws_recv_frame(req, &ws_pkt, ws_pkt.len);
    if (ret != ESP_OK) {
        printf("failed to receive frame: %s\n", esp_err_to_name(ret));
        free(buf);
        return ret;
    }
    printf("Received message: %s\n", (char *)ws_pkt.payload);
    if (strstr((char *)ws_pkt.payload, "on")) {
        gpio_set_level(LED, 1);
    } else if (strstr((char *)ws_pkt.payload, "off")) {
        gpio_set_level(LED, 0);
    }

    httpd_ws_frame_t resp = {
        .type = HTTPD_WS_TYPE_TEXT,
        .payload = ws_pkt.payload,
        .len = ws_pkt.len};

    httpd_ws_send_frame(req, &resp);
    free(buf);
    return ESP_OK;
}

static const char html_page[] =
"<!DOCTYPE html><html>"
"<head><meta charset='UTF-8'><title>ESP32 WebSocket</title></head>"
"<body><h1>WebSocket LED Control</h1>"
"<button onclick=\"send('on')\">LED ON</button>"
"<button onclick=\"send('off')\">LED OFF</button>"
"<script>"
"var ws = new WebSocket('ws://' + window.location.host + '/ws');"
"ws.onmessage = function(e){ console.log('From ESP:', e.data); };"
"function send(msg){ ws.send(msg); }"
"</script></body></html>";

static esp_err_t root_get_handler(httpd_req_t *req) {
    httpd_resp_send(req, html_page, HTTPD_RESP_USE_STRLEN);
    return  ESP_OK;
}

static const httpd_uri_t ws_uri = {
    .uri = "/ws",
    .method = HTTP_GET,
    .handler = ws_handler,
    .is_websocket = true
};

static const httpd_uri_t root_uri = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = root_get_handler
};

static httpd_handle_t start_webserver(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    httpd_handle_t server = NULL;
    if (httpd_start(&server, &config)==ESP_OK) {
        httpd_register_uri_handler(server, &root_uri);
        httpd_register_uri_handler(server, &ws_uri);
        return server;
    }
    printf("Error starting server~\n");
    return NULL;
}

static void wifi_init_softap(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = "ESP32_WS_AP",
            .ssid_len = 0,
            .channel = 1,
            .password = "12345678",
            .max_connection = 2,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    printf("Wifi AP started, SSID:%s\n", wifi_config.ap.ssid);
}


void app_main(void) {
    gpio_reset_pin(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    ESP_ERROR_CHECK(nvs_flash_init());
    wifi_init_softap();
    start_webserver();
}



