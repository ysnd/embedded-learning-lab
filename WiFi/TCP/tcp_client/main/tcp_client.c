#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include "esp_event_base.h"
#include "esp_netif.h"
#include "esp_netif_ip_addr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"

#define SSID "yourssid"
#define PASS "yourpassword"
#define SERVER_IP "192.168.4.1"
#define SERVER_PORT 3333

static EventGroupHandle_t wifi_event_group;
#define  WIFI_CONNECTED_BIT BIT0

static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        printf("Disconected. reconnecting...\n");
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        printf("Got Ip: " IPSTR "\n", IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

static void wifi_init_sta(void) {
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // register event handlers
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = SSID,
            .password = PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

static void tcp_client_task(void *pvParameters) {
    char rx_buffer[128];
    char tx_buffer[128];
    int addr_family = AF_INET;
    int ip_protocol = IPPROTO_IP;
    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(SERVER_PORT);

    while (1) {
        int sock = socket(addr_family, SOCK_STREAM, ip_protocol);
        if (sock < 0) {
            printf("Unable to create socket: errno %d\n", errno);
            vTaskDelay(2000 / portTICK_PERIOD_MS);
            continue;
        }
        printf("Socket created, connecting to %s:%d\n",SERVER_IP, SERVER_PORT);
        int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err != 0) {
            printf("Socket unable to connect: errno %d\n", errno);
            close(sock);
            vTaskDelay(2000 / portTICK_PERIOD_MS);
            continue;
        }

        printf("Successfully connected\n");

        while (1) {
            snprintf(tx_buffer, sizeof(tx_buffer), "Hallo ACAB from ESP32!\n");
            int err = send(sock, tx_buffer, strlen(tx_buffer), 0);
            if (err < 0) {
                printf("Error occurred during sending: errno %d\n", errno);
                break;
            }

            int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
            if (len < 0) {
                printf("recv failed: errno %d\n", errno);
                break;
            } else if (len == 0) {
                printf("Connection closed\n");
                break;
            } else {
                rx_buffer[len] = 0; // null terminate
                printf("Received: %s\n", rx_buffer);
            }

            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }

        close(sock);
        printf("Socket closed, retrying in 5s...\n");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

void app_main(void) {
    ESP_ERROR_CHECK(nvs_flash_init());
    wifi_init_sta();

    xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT, false, true, portMAX_DELAY);
    printf("Wifi Conected, stating  TCP client task...\n");
    xTaskCreate(tcp_client_task, "tcp cvlient", 4096, NULL, 5, NULL);
}
