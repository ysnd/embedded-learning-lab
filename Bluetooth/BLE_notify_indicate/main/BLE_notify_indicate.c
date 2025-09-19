#include <stdbool.h>
#include <stdio.h>
#include "esp_bt_main.h"
#include "esp_gatt_defs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_gatt_common_api.h"
#include "driver/adc.h"

#define WTRLVL ADC1_CHANNEL_6

static uint16_t primary_service_uuid = ESP_GATT_UUID_PRI_SERVICE;
static uint16_t character_declaration_uuid = ESP_GATT_UUID_CHAR_DECLARE;
static uint16_t character_client_config_uuid = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;

//characteristic props read notyfi indicate
static uint8_t char_prop_read_notify_indicate = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY | ESP_GATT_CHAR_PROP_BIT_INDICATE;

static uint16_t SERVICE_UUID = 0x00FF;
static uint16_t CHAR_UUID = 0xFF01;
static uint16_t service_handle = 0;

//handle cjar
static uint16_t adc_char_handle = 0;

//coninfo
static esp_gatt_if_t gatts_if_for_conn = 0;
static uint16_t conn_id_for_conn = 0;

// adv parameter
static esp_ble_adv_params_t adv_params = {
    .adv_int_min = 0x20,
    .adv_int_max = 0x50,
    .adv_type = ADV_TYPE_IND,
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
    .channel_map = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

static esp_ble_adv_data_t adv_data = {
   .set_scan_rsp = false,
    .include_name = true,
    .include_txpower = true,
    .min_interval = 0x20,
    .max_interval = 0x40,
    .appearance = 0x00,
    .manufacturer_len = 0,
    .p_manufacturer_data = NULL,
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = 0,
    .p_service_uuid = NULL,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

//gatt database
static const esp_gatts_attr_db_t gatt_db[] = {
    //service declaration   
    [0] = {
        {ESP_GATT_AUTO_RSP}, 
        {ESP_UUID_LEN_16, (uint8_t *)&primary_service_uuid, ESP_GATT_PERM_READ, sizeof(uint16_t), sizeof(SERVICE_UUID), (uint8_t *)&SERVICE_UUID}
    },
    //char declaration  
    [1] = {
        {ESP_GATT_AUTO_RSP},
        {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ, sizeof(uint8_t), sizeof(uint8_t), (uint8_t *)&char_prop_read_notify_indicate}
    },
    //characteristic value
    [2] = {
        {ESP_GATT_AUTO_RSP},
        {ESP_UUID_LEN_16, (uint8_t *)&CHAR_UUID, ESP_GATT_PERM_READ, sizeof(uint16_t), 0, NULL}
    },
    [3] = {
        {ESP_GATT_AUTO_RSP},
        {ESP_UUID_LEN_16, (uint8_t *)&character_client_config_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, sizeof(uint16_t), 0, NULL}
    },
};

static void adc_init(void) {
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(WTRLVL, ADC_ATTEN_DB_12);
    printf("ADC initialized for water level reading on GPIO33\n");
}

static uint8_t read_wtr_level(void) {
    int raw = adc1_get_raw(WTRLVL);
    uint8_t water_percentage = (raw * 100) / 4095;
    return water_percentage;
}

//gatts server callback
static void gatts_cb(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) {
    switch (event) {
        case ESP_GATTS_REG_EVT:
            printf("REGISTER_APP_EVT\n");
            esp_ble_gap_config_adv_data(&adv_data);
            esp_ble_gap_set_device_name("ACAB!");
            esp_ble_gatts_create_attr_tab(gatt_db, gatts_if, 4, 0);
            break;

        case ESP_GATTS_CREAT_ATTR_TAB_EVT:
            if (param->add_attr_tab.status == ESP_GATT_OK) {
                service_handle = param->add_attr_tab.handles[0];
                adc_char_handle = param->add_attr_tab.handles[2];
                esp_ble_gatts_start_service(service_handle);       
            } else {
                printf("create atribute table failed, error 0x%x\n", param->add_attr_tab.status);
            }
            break;

        case ESP_GATTS_CONNECT_EVT:
            printf("Device connected\n");
            gatts_if_for_conn = gatts_if;
            conn_id_for_conn = param->connect.conn_id;
            break;

        case ESP_GATTS_DISCONNECT_EVT:
            printf("Device disconected, restart adv\n");
            esp_ble_gap_start_advertising(&adv_params);
            break;

        default:
            break;
    } 
}

static void gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
    switch (event) {
        case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
            esp_ble_gap_start_advertising(&adv_params);
            break;
        default:
            break;
    }
}

void adc_task(void *arg) {
    while (1) {
        if (gatts_if_for_conn !=0) {
            int wtr_val = read_wtr_level();
            uint8_t value[2];
            value[0] = wtr_val & 0xFF;
            value[1] = (wtr_val >>8) & 0xFF;

            esp_ble_gatts_send_indicate(
                    gatts_if_for_conn, 
                    conn_id_for_conn, 
                    adc_char_handle, 
                    sizeof(value), 
                    value,
                    true);//false = notyfy, true = indicate
            printf("Level sent : %d\n", wtr_val);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void) {
    esp_err_t ret;

    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);


    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BLE));

    ESP_ERROR_CHECK(esp_bluedroid_init());
    ESP_ERROR_CHECK(esp_bluedroid_enable());


    ESP_ERROR_CHECK(esp_ble_gap_register_callback(gap_cb));
    ESP_ERROR_CHECK(esp_ble_gatts_register_callback(gatts_cb));
    ESP_ERROR_CHECK(esp_ble_gatts_app_register(0));
    adc_init();

    xTaskCreate(adc_task, "adc_task", 4096, NULL, 5, NULL);
    printf("BLE notify indicate GATT Server Started...\n");
}



