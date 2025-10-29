#include <string.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_http_server.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

#define SSID "lbndr"
#define PASS "password"

#define LED 2
#define SERVO 18

static EventGroupHandle_t wifi_event_group;
#define WIFI_CONNECTED_BIT BIT0

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        printf("Disconnected. Reconnecting...\n");
        esp_wifi_connect();
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        printf("Got IP: " IPSTR "\n", IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

static void servo_init(void) {
    ledc_timer_config_t timer_cfg = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = LEDC_TIMER_16_BIT,
        .freq_hz = 50,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer_cfg);

    ledc_channel_config_t ch_cfg = {
        .gpio_num = SERVO,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = BIT0};
    ledc_channel_config(&ch_cfg);
}
//angle 0-180 duty 500-250us
static void servo_set_angle(int angle) {
    int duty_us = 500 + (angle * 2000 / 180);
    int max_duty = (1 << 16) - 1;
    int duty_val = (duty_us * max_duty) / 20000;
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty_val);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}

static esp_err_t root_get_handler(httpd_req_t *req) {
    const char *resp_str = 
        "<html>"
        "<head><title>ESP32 Control</title></head>"
        "<body>"
        "<h2>ESP32 Web Control ACAB!!</h2>"
        "<p><a href=\"/led?action=on\">LED ON</a> | <a href=\"/led?action=off\">LED OFF</a?></p>"
        "<p><a href=\"/servo?angle=0\">Servo 0</a> | "
        "<a href=\"/servo?angle=90\">Servo 90</a> | "
        "<a href=\"/servo?angle=180\">Servo 180</a></p>"
        "</body></html>";

    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static esp_err_t led_handler(httpd_req_t *req) {
    char param[16];
    if (httpd_req_get_url_query_str(req, param, sizeof(param))==ESP_OK) {
        char value[16];
        if (httpd_query_key_value(param, "action", value, sizeof(value))==ESP_OK) {
            if (strcmp(value, "on")==0) {
                gpio_set_level(LED, 1);
                printf("LED ON\n");
            } else if (strcmp(value, "off")==0) {
                gpio_set_level(LED, 0);
                printf("LED OFF\n");
            }
        }
    }
    httpd_resp_sendstr(req, "OK");
    return ESP_OK;
}


static esp_err_t servo_handler(httpd_req_t *req) {
    char param[32];
    if (httpd_req_get_url_query_str(req, param, sizeof(param))==ESP_OK) {
        char value[16];
        if (httpd_query_key_value(param, "angle", value, sizeof(value))==ESP_OK) {
            int angle = atoi(value);
            if (angle >= 0 && angle <= 180) {
                servo_set_angle(angle);
                printf("Servo angle: %d\n",angle);
            }
        }
    }
    httpd_resp_sendstr(req, "OK");
    return ESP_OK;
}

//webserver
static httpd_handle_t start_webserver(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config)==ESP_OK) {
        httpd_uri_t uri_root = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = root_get_handler
        };
        httpd_uri_t uri_led = {
            .uri = "/led",
            .method = HTTP_GET,
            .handler = led_handler
        };
        httpd_uri_t uri_servo = {
            .uri = "/servo",
            .method = HTTP_GET,
            .handler = servo_handler
        };
        httpd_register_uri_handler(server, &uri_root);
        httpd_register_uri_handler(server, &uri_led);
        httpd_register_uri_handler(server, &uri_servo);
    }
    return server;
}

void app_main(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = SSID,
            .password = PASS,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    printf("Connecting to Wi-Fi...\n");
    xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT, pdFALSE, pdTRUE, portMAX_DELAY);

    gpio_config_t io_cfg = {
        .pin_bit_mask = 1ULL << LED,
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = 0,
        .pull_up_en = 0 
    };
    gpio_config(&io_cfg);
    servo_init();
    start_webserver();
}

