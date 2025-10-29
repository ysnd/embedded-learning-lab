#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_http_server.h"
#include "driver/gpio.h"

#define SSID "ESP32_NAtive_real"
#define PASS "12344321"
#define MAX_STA_CONN 4
#define LED 2

static const char *index_html =
    "<!doctype html>\n"
    "<html>\n"
    "<head>\n"
    "  <meta charset='utf-8'>\n"
    "  <meta name='viewport' content='width=device-width,initial-scale=1'>\n"
    "  <title>ESP32 Native UI</title>\n"
    "  <style>body{font-family:sans-serif;text-align:center;background:#111;color:#eee}button{padding:12px 20px;margin:8px;border-radius:8px}</style>\n"
    "</head>\n"
    "<body>\n"
    "  <h2>ESP32 Native HTTP Server</h2>\n"
    "  <div>\n"
    "    <button onclick=\"fetch('/control',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({action:'on'})})\">LED ON</button>\n"
    "    <button onclick=\"fetch('/control',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({action:'off'})})\">LED OFF</button>\n"
    "  </div>\n"
    "  <p>Status: <strong id='led'>N/A</strong></p>\n"
    "  <p>Clients: <strong id='clients'>0</strong></p>\n"
    "  <hr>\n"
    "  <p>Realtime updates via Server-Sent Events</p>\n"
    "  <script>\n"
    "    const evtSource = new EventSource('/events');\n"
    "    evtSource.onmessage = function(e){\n"
    "      try{\n"
    "        const d = JSON.parse(e.data);\n"
    "        document.getElementById('led').innerText = d.led;\n" 
    "        document.getElementById('clients').innerText = d.clients;\n" 
    "      }catch(err){console.warn(err);}    };\n"
    "    evtSource.onerror = function(e){ console.warn('SSE error', e); };\n" 
    "  </script>\n"
    "</body>\n"
    "</html>\n";

static volatile int led_state = 0;

static int get_ap_client_count(void)
{
    wifi_sta_list_t sta_list;
    if (esp_wifi_ap_get_sta_list(&sta_list) != ESP_OK) return 0;
    return sta_list.num;
}

static void wifi_init_softap(void)
{
    printf("Init network stack...\n");
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t ap_conf = {
        .ap = {
            .ssid = SSID,
            .ssid_len = strlen(SSID),
            .password = PASS,
            .channel = 1,
            .max_connection = MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        }
    };
    if (strlen(PASS) == 0) ap_conf.ap.authmode = WIFI_AUTH_OPEN;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_conf));
    ESP_ERROR_CHECK(esp_wifi_start());

    printf("AP started SSID:%s PASSWORD:%s\n", SSID, PASS);
    printf("Connect a client and open http://192.168.4.1/\n");
}

static esp_err_t root_get_handler(httpd_req_t *req) {
    httpd_resp_set_type(req, "text/html; charset=utf-8");
    httpd_resp_send(req, index_html, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static esp_err_t status_get_handler(httpd_req_t *req) {
    char buf[128];
    int clients = get_ap_client_count(); 
    snprintf(buf, sizeof(buf), "{\"led\":\"%s\",\"clients\":%d}", led_state ? "ON" : "OFF", clients);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, buf, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}
static esp_err_t control_post_handler(httpd_req_t *req) {
    char buf[128];
    int ret, remaining = req->content_len;
    if (remaining <= 0) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "No Body");
        return ESP_FAIL;
    }
    if (remaining >= (int)sizeof(buf)) remaining = sizeof(buf) - 1;
    ret = httpd_req_recv(req, buf, remaining);
    if (ret <= 0) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "recv failed");
        return ESP_FAIL;
    }
    buf[ret]= '\0';

    char *action = NULL;
    if (strstr(buf, "\"action\"") !=NULL) {
        char *p = strstr(buf, "\"action\"");
        p = strchr(p, ':');
        if (p) {
            p++;
            while (*p == ' ' || *p=='\"' || *p == '\'') p++;
            char *q=p;
            while (*q && *q != '\"' && *q != '\'' && *q != ' ' && *q != '}' && *q != '&') q++;
            int len = q - p;
            action = malloc(len + 1);
            if (action) {
                memcpy(action, p, len);
                action[len] = '\0';
            }
        }
    } else {
        /* urlencoded: action=on */
        char *p = strstr(buf, "action=");
        if (p) {
            p += strlen("action=");
            char *q = strchr(p, '&');
            int len = q ? (q - p) : strlen(p);
            action = malloc(len + 1);
            if (action) {
                memcpy(action, p, len);
                action[len] = '\0';
            }
        }
    }

    if (!action) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "action missing");
        return ESP_FAIL;
    }

    if (strcasecmp(action, "on") == 0) {
        gpio_set_level(LED, 1);
        led_state = 1;
        printf("LED ON via /control\n");
    } else if (strcasecmp(action, "off") == 0) {
        gpio_set_level(LED, 0);
        led_state = 0;
        printf("LED OFF via /control\n");
    } else {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "unknown action");
        free(action);
        return ESP_FAIL;
    }
    free(action);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr(req, "{\"result\":\"OK\"}");
    return ESP_OK;
}

static esp_err_t sse_handler(httpd_req_t *req)
{
    httpd_resp_set_hdr(req, "Content-Type", "text/event-stream");
    httpd_resp_set_hdr(req, "Cache-Control", "no-cache");
    httpd_resp_set_hdr(req, "Connection", "keep-alive");
    const char *retry = "retry: 2000\n\n";
    if (httpd_resp_send_chunk(req, retry, strlen(retry)) != ESP_OK) {
        printf("Client closed (initial retry)\n");
        return ESP_FAIL;
    }

    for (;;) {
        char payload[128];
        int clients = get_ap_client_count();
        snprintf(payload, sizeof(payload), "data:{\"led\":\"%s\",\"clients\":%d}\n\n", led_state ? "ON" : "OFF", clients);

        esp_err_t err = httpd_resp_send_chunk(req, payload, strlen(payload));
        if (err != ESP_OK) {
            printf("SSE client disconnected\n");
            break;
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

static httpd_handle_t start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 80;
    config.stack_size = 4096;

    httpd_handle_t server = NULL;
    if (httpd_start(&server, &config) != ESP_OK) {
        printf("Failed to start HTTP server\n");
        return NULL;
    }

    httpd_uri_t uri_root = {
        .uri       = "/",
        .method    = HTTP_GET,
        .handler   = root_get_handler,
        .user_ctx  = NULL
    };
    httpd_register_uri_handler(server, &uri_root);

    httpd_uri_t uri_status = {
        .uri       = "/status",
        .method    = HTTP_GET,
        .handler   = status_get_handler
    };
    httpd_register_uri_handler(server, &uri_status);

    httpd_uri_t uri_control = {
        .uri       = "/control",
        .method    = HTTP_POST,
        .handler   = control_post_handler
    };
    httpd_register_uri_handler(server, &uri_control);

    httpd_uri_t uri_events = {
        .uri       = "/events",
        .method    = HTTP_GET,
        .handler   = sse_handler
    };
    httpd_register_uri_handler(server, &uri_events);

    printf("HTTP server started\n");
    return server;
}


void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    gpio_reset_pin(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    gpio_set_level(LED, 0);

    wifi_init_softap();

    vTaskDelay(pdMS_TO_TICKS(1000));
    start_webserver();
}


