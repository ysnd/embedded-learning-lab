#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_spp_api.h"

#define DEVICE_NAME "ESP_TEST-SPP"

static bool spp_initialized = false;

static void spp_callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
    printf("SPP Event: %d\n", event);

    switch (event) {
        case ESP_SPP_INIT_EVT:
            printf("SPP Init Event\n");
            spp_initialized = true;
            esp_bt_gap_set_device_name(DEVICE_NAME);
            esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
            esp_spp_start_srv(ESP_SPP_SEC_NONE, ESP_SPP_ROLE_SLAVE, 0, "SPP");
            break;
        case ESP_SPP_START_EVT:
            printf("SPP Server Started!\n");
            break;
        case ESP_SPP_SRV_OPEN_EVT:
            printf("Client Connected\n");
            break;
        case ESP_SPP_DATA_IND_EVT:
            printf("Data: %.*s\n", param->data_ind.len, param->data_ind.data);
            break;
        case ESP_SPP_CLOSE_EVT:
            printf("Client Disconected\n");
            break;
        default:
            break;
    }
}

void app_main(void) {
    printf("ESP32 SPP Test Starting...\n");
    nvs_flash_init();
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    esp_bt_controller_init(&bt_cfg);
    esp_bt_controller_enable(ESP_BT_MODE_BTDM);

    printf("Controller ready\n");
    vTaskDelay(pdMS_TO_TICKS(500));

    esp_bluedroid_init();
    esp_bluedroid_enable();
    printf("Bluedroid ready\n");
    vTaskDelay(pdMS_TO_TICKS(500));

    printf("registering spp callback...\n");
    esp_spp_register_callback(spp_callback);

    vTaskDelay(pdMS_TO_TICKS(200));
    printf("Starting SPP init..\n");
    esp_err_t result = esp_spp_init(ESP_SPP_MODE_CB);
    printf("SPP init result: %d\n", result);

    if (result == ESP_OK) {
        printf("SUCCES! SPP Initialized\n");
    } else {
        printf("FAILED! SPP Init error\n");
        return;
    }

    int timeout = 0;
    while (!spp_initialized && timeout < 50) {
        vTaskDelay(pdMS_TO_TICKS(100));
        timeout++;
    }

    if (spp_initialized) {
        printf("\n--------- ESP32 Ready---------\n");
        printf("Device: %s\n", DEVICE_NAME);
        printf("Look for it in Bluetooth setting\n");
    } else {
        printf("Timeout waiting for SPP init\n");
    }
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }   
}
