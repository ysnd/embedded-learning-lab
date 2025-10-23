#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_mac.h"
#include "nvs_flash.h"

#define STA_SSID "yourssid"
#define STA_PASS "yourpassword"
#define AP_SSID "ESP32_AP_mode"
#define AP_PASS "12344321"

static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT) {
        switch (event_id) {
            case WIFI_EVENT_STA_START:
                printf("[STA] Connecting to WiFi...\n");
                esp_wifi_connect();
                break;
            case WIFI_EVENT_STA_CONNECTED:
                printf("[STA] Connected to router\n");
                break;
            case WIFI_EVENT_STA_DISCONNECTED:
                printf("[STA] Disconected, Reconnecting...\n");
                break;
            case WIFI_EVENT_AP_STACONNECTED: {
                wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *) event_data;
                printf("[AP] Device Connected: " MACSTR "\n", MAC2STR(event->mac));
                break;
                }
            case WIFI_EVENT_AP_STADISCONNECTED: {
                wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *) event_data;
                printf("[AP] Device disconnected: " MACSTR "\n", MAC2STR(event->mac));
                break;
                }
            default:
                break;                                
        }
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
        printf("[STA] Got IP: " IPSTR "\n", IP2STR(&event->ip_info.ip));
    }
}

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    //init network stack
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    //create interface for sta & ap 
    esp_netif_create_default_wifi_sta();
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    //register event handler    
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL));

    //config mode wifi  
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));

    //sta config    
    wifi_config_t sta_cfg = {
        .sta = {
            .ssid = STA_SSID,
            .password = STA_PASS,
            .threshold.authmode 
                = WIFI_AUTH_WPA2_PSK,
        },
    };

    //AP Config 
    wifi_config_t ap_cfg = {
        .ap = {
            .ssid = AP_SSID,
            .ssid_len = strlen(AP_SSID),
            .password = AP_PASS,
            .max_connection = 4,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK,
            .channel = 6,
        },
    };
    if (strlen(AP_PASS) == 0) {
        ap_cfg.ap.authmode = WIFI_AUTH_OPEN;
    }
    //apply config
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_cfg));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_cfg));

    //start wifi
    ESP_ERROR_CHECK(esp_wifi_start());

    printf("Wifi APSTA started. \n");
    printf("AP SSID: %s | Pass: %s\n", AP_SSID, AP_PASS);
    printf("STA SSID: %s | PASS: %s\n", STA_SSID, STA_PASS);
    
}
