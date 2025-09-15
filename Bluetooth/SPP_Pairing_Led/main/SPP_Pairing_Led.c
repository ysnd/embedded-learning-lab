#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_spp_api.h"

#define DEVICE_NAME "ESP32_SPP_LED"
#define LED GPIO_NUM_2

static void gap_callback(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param) {
    switch (event) {
        case ESP_BT_GAP_AUTH_CMPL_EVT:
            if (param->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS) {
                printf("Auth succes, peer: %s\n", param->auth_cmpl.device_name);
            } else {
                printf("Auth failed, status: %d\n", param->auth_cmpl.stat);
            }
            break;
        case ESP_BT_GAP_PIN_REQ_EVT:
            printf("Pin code request event\n");
            esp_bt_pin_code_t pin;
            pin[0] = '1'; pin[1] = '2'; pin[2] = '3'; pin[3] = '4';
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 4, pin);
            printf("Replied with PIN: 1234\n");
            break;

        default:
            printf("GAP Event: %d\n", event);
            break;
    }
}

//spp calback
static void spp_callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
    switch (event) {
        case ESP_SPP_INIT_EVT:
            printf("SPP Init Event\n");
            esp_bt_gap_register_callback(gap_callback);
            esp_bt_gap_set_device_name(DEVICE_NAME);
            esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
            esp_spp_start_srv(ESP_SPP_SEC_AUTHENTICATE, ESP_SPP_ROLE_SLAVE, 0, "SPP");
            break;
        case ESP_SPP_START_EVT:
            printf("SPP Server Started!\n");
            break;
        case ESP_SPP_SRV_OPEN_EVT:
            printf("Client Connected\n");
            break;
        case ESP_SPP_DATA_IND_EVT: {
            printf("Data received (%d): %.*s\n", param->data_ind.len,
                   param->data_ind.len, param->data_ind.data);
            if (strncmp((char *)param->data_ind.data, "LED_ON", 6) == 0) {
                gpio_set_level(LED, 1);
                char *msg = "LED ON !\n";
                esp_spp_write(param->data_ind.handle, strlen(msg), (uint8_t *)msg);
                printf("LED ON\n");
            } else if (strncmp((char *)param->data_ind.data, "LED_OFF", 7) == 0 ) {
                gpio_set_level(LED, 0);
                char *msg = "LED OFF !\n";
                esp_spp_write(param->data_ind.handle, strlen(msg), (uint8_t *)msg);
                printf("LED OFF\n");
            }
            break;
        }

        case ESP_SPP_CLOSE_EVT:
            printf("Client Disconected\n");
            break;
        default:
            break;
    }
}


void app_main(void)
{
    printf("ESP32 SPP Secure LED Control Starting...\n");

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }

    gpio_reset_pin(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    gpio_set_level(LED, 0);

    esp_bt_controller_config_t bt_conf = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    esp_bt_controller_init(&bt_conf);
    esp_bt_controller_enable(ESP_BT_MODE_BTDM);

    esp_bluedroid_init();
    esp_bluedroid_enable();

    esp_spp_register_callback(spp_callback);
    esp_spp_cfg_t spp_cfg = { 0 };
    spp_cfg.mode = ESP_SPP_MODE_CB;     
    spp_cfg.enable_l2cap_ertm = false;    
    spp_cfg.tx_buffer_size = 0;            

    ret = esp_spp_enhanced_init(&spp_cfg);
    printf("esp_spp_enhanced_init: %s\n", esp_err_to_name(ret));
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

