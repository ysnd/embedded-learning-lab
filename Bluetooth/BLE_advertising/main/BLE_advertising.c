#include <stdio.h>
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_bt_main.h"
#include "nvs_flash.h"

#define DEVICE_NAME "ESPBLE_DEMOO"

static void gap_callback(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
    if (event == ESP_GAP_BLE_ADV_START_COMPLETE_EVT) {
        printf("Advertising started\n");
    }
}


void app_main(void)
{
    nvs_flash_init();
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    esp_bt_controller_init(&bt_cfg);
    esp_bt_controller_enable(ESP_BT_MODE_BLE);

    esp_bluedroid_init();
    esp_bluedroid_enable();

    esp_ble_gap_register_callback(gap_callback);

    esp_ble_adv_data_t adv_data = {
        .set_scan_rsp = false,
        .include_name = true,
        .include_txpower = true,
        .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
    };
    esp_ble_gap_config_adv_data(&adv_data);

    esp_ble_adv_params_t adv_params = {
        .adv_int_min = 0x20,
        .adv_int_max = 0x40,
        .adv_type = ADV_TYPE_IND,
        .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
        .channel_map = ADV_CHNL_ALL,
        .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
    };
    esp_ble_gap_start_advertising(&adv_params);
}
