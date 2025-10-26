#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"

#define WIFI_SSID "yourssid"
#define WIFI_PASS "yourpassword"

#define SERVER_IP "192.168.4.1"
#define SERVER_PORT 3333
#define LOCAL_PORT 3333           

static EventGroupHandle_t wifi_event_group;
#define WIFI_CONNECTED_BIT BIT0

static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } 
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        printf("Disconnected. Reconnecting...\n");
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
    } 
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        printf("Got IP: " IPSTR "\n", IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

static void wifi_init_sta(void)
{
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

static void udp_client_task(void *pvParameters)
{
    char rx_buffer[128];
    char tx_buffer[128];
    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(SERVER_PORT);

    struct sockaddr_in local_addr;
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(LOCAL_PORT);

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock < 0) {
        printf("Unable to create socket: errno %d\n", errno);
        vTaskDelete(NULL);
    }

    if (bind(sock, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0) {
        printf("Socket unable to bind: errno %d\n", errno);
        close(sock);
        vTaskDelete(NULL);
    }

    printf("UDP socket created, bound to port %d\n", LOCAL_PORT);

    while (1) {
        snprintf(tx_buffer, sizeof(tx_buffer), "Ping from ESP32!\n");
        int err = sendto(sock, tx_buffer, strlen(tx_buffer), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err < 0) {
            printf("Error occurred during sending: errno %d\n", errno);
        } else {
            printf("Sent: %s\n", tx_buffer);
        }

        struct sockaddr_in source_addr;
        socklen_t socklen = sizeof(source_addr);
        int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&source_addr, &socklen);

        if (len < 0) {
            printf("recvfrom failed: errno %d\n", errno);
        } else {
            rx_buffer[len] = 0;
            printf("Received %d bytes from %s: %s\n", len, inet_ntoa(source_addr.sin_addr), rx_buffer);
        }

        vTaskDelay(pdMS_TO_TICKS(2000));
    }

    close(sock);
    vTaskDelete(NULL);
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    wifi_init_sta();
    xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT, false, true, portMAX_DELAY);
    printf("WiFi connected, starting UDP communication...\n");

    xTaskCreate(udp_client_task, "udp_client", 4096, NULL, 5, NULL);
}

