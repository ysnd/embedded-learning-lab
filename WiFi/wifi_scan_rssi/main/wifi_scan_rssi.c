#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"

static const char *TAG = "WIFI_RSSI_TASK";
static TaskHandle_t wifi_scan_task_handle = NULL;
static volatile bool scan_in_progress = false;  

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_SCAN_DONE)
    {
        scan_in_progress = false;
        uint16_t ap_count = 0;
        ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));

        wifi_ap_record_t ap_records[20];
        uint16_t num = (ap_count < 20) ? ap_count : 20;
        ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&num, ap_records));

        ESP_LOGI(TAG, "Scan selesai, ditemukan %d Access Point", ap_count);
        for (int i = 0; i < num; i++)
        {
            ESP_LOGI(TAG, "[%2d] SSID: %-20s | RSSI: %4d dBm | CH: %2d",
                     i + 1, ap_records[i].ssid, ap_records[i].rssi, ap_records[i].primary);
        }
    }
}

void wifi_scan_task(void *pvParameters)
{
    wifi_scan_config_t scan_config = {
        .ssid = 0,
        .bssid = 0,
        .channel = 0,
        .show_hidden = true
    };

    while (1)
    {
        if (!scan_in_progress)
        {
            esp_err_t err = esp_wifi_scan_start(&scan_config, false);
            if (err == ESP_OK)
            {
                scan_in_progress = true;
                ESP_LOGI(TAG, "Memulai scan Wi-Fi...");
            }
            else
            {
                ESP_LOGE(TAG, "Gagal memulai scan: %s", esp_err_to_name(err));
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

static void wifi_init(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
}

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    wifi_init();

    xTaskCreate(wifi_scan_task, "wifi_scan_task", 8192, NULL, 5, &wifi_scan_task_handle);

    while (1)
    {
        ESP_LOGI(TAG, "Main task masih jalan...");
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}





