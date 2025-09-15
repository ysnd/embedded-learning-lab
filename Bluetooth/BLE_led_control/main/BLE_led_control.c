#include <stdio.h>
#include <string.h>
#include "esp_err.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_gatt_defs.h"
#include "esp_gatt_common_api.h"

#define LED 2

//uuid
#define SERVICE_UUID 0x00FF
#define CHAR_UUID 0xFF01

static esp_ble_adv_params_t adv_params = {
    .adv_int_min = 0x20,
    .adv_int_max = 0x40,
    .adv_type = ADV_TYPE_IND,
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
    .channel_map = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY
};

static uint16_t service_handle = 0;
static esp_gatt_if_t gl_gatts_if = 0;
static uint16_t gl_conn_id = 0;
static uint16_t char_handle = 0;
static uint16_t cccd_handle = 0;
static bool notify_enabled = false;
static bool connected = false;

static void make_uuid16(esp_bt_uuid_t *uuid, uint16_t u16) {
    uuid->len = ESP_UUID_LEN_16;
    uuid->uuid.uuid16 = u16;
}

//gap callback
static void gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
    switch (event) {
    case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
        printf("adv data set complete -> start adv\n");
        esp_ble_gap_start_advertising(&adv_params);
        break;
    default:
        break;
    }
}

// gatts event handler
static void gatts_cb(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) {
    esp_err_t err;
    switch (event) {
    case ESP_GATTS_REG_EVT:
        printf("GATTS_REG_EVT, appp_id %d\n", param->reg.app_id);

        //set device name
        esp_ble_gap_set_device_name("ESP32_LED_BLE");
        //minimal adv data: include name +service uuid16
        {
            uint8_t service_uuid128[16] = {
                0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, (SERVICE_UUID & 0xFF), (SERVICE_UUID >> 8), 0x00, 0x00  
            };
            esp_ble_adv_data_t adv_data = {
                .set_scan_rsp = false,
                .include_name = true,
                .include_txpower = true,
                .service_uuid_len = sizeof(service_uuid128),
                .p_service_uuid = service_uuid128,
                .flag = ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT,
            };
            esp_ble_gap_config_adv_data(&adv_data);// triggers ADV_DATA_SET_COMPLETE event
        }
        //create service
        {
            esp_gatt_srvc_id_t srvc_id;
            memset(&srvc_id, 0, sizeof(srvc_id));
            srvc_id.is_primary = true;
            srvc_id.id.inst_id = 0;
            make_uuid16(&srvc_id.id.uuid, SERVICE_UUID);

            err = esp_ble_gatts_create_service(gatts_if, &srvc_id, 4);
            if (err != ESP_OK) {
                printf("create service failed : %s\n", esp_err_to_name(err));
            }
        }
        break;
    
    case ESP_GATTS_CREATE_EVT:
        printf("CREATE_EVT, status %d, service_handle %d\n", param->create.status, param->create.service_handle);
        service_handle = param->create.service_handle;

        //start service
        esp_ble_gatts_start_service(service_handle);

        //add caracteristic (read/write/notify)
        {
            esp_bt_uuid_t char_uuid;
            make_uuid16(&char_uuid, CHAR_UUID);
            esp_attr_control_t control = {0};
            esp_attr_value_t gatts_char_val = {
                .attr_max_len = 20,
                .attr_len = 1,
                .attr_value = (uint8_t[]){0}
            };
            err = esp_ble_gatts_add_char(service_handle, &char_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_NOTIFY, &gatts_char_val, &control);
            if (err != ESP_OK) {
                printf("add char failed : %s\n", esp_err_to_name(err));
            }
        }
        break;

    case ESP_GATTS_ADD_CHAR_EVT:
        printf("ADD_CHAR_EVT, attr_handle %d\n", param->add_char.attr_handle);
        char_handle = param->add_char.attr_handle;
        //add cccd descriptor for notify
        {
            esp_bt_uuid_t descr_uuid;
            make_uuid16(&descr_uuid, ESP_GATT_UUID_CHAR_CLIENT_CONFIG); //0x2902    
            esp_attr_value_t descr_val = {
                .attr_max_len = 2,
                .attr_len = 2,
                .attr_value = (uint8_t[]){0x00, 0x00}
            };
            err = esp_ble_gatts_add_char_descr(service_handle, &descr_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, &descr_val, NULL);
            if (err != ESP_OK) {
                printf("add_descr failed : %s\n", esp_err_to_name(err));
            }
        }
        break;
    case ESP_GATTS_ADD_CHAR_DESCR_EVT:
        printf("ADD_DESCR_EVT, hande %d\n", param->add_char_descr.attr_handle);
        cccd_handle = param->add_char_descr.attr_handle;
        break;
    case ESP_GATTS_START_EVT:
        printf("SERVICE START_EVT\n");
        break;

    case ESP_GATTS_CONNECT_EVT:
        printf("CONNECT_EVT, conn_id %d\n", param->connect.conn_id);
        gl_gatts_if = gatts_if;
        gl_conn_id = param->connect.conn_id;
        connected = true;
        notify_enabled = false; //reset notify new conection
        break;

    case ESP_GATTS_DISCONNECT_EVT:
        printf("Disconect_evt\n");
        connected = false;
        notify_enabled = false;
        gl_conn_id = 0;
        //restart adv   
        esp_ble_gap_start_advertising(&adv_params);
        break;

    case ESP_GATTS_WRITE_EVT:
        printf("WRITE_EVT handle=%d, len=%d\n", param->write.handle, param->write.len);
        //send write response first
        if (param->write.need_rsp) {
            esp_gatt_rsp_t rsp;
            memset(&rsp, 0, sizeof(esp_gatt_rsp_t));
            rsp.attr_value.handle = param->write.handle;
            rsp.attr_value.len = param-> write.len;
            memcpy(rsp.attr_value.value, param->write.value, param->write.len);
            esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, ESP_GATT_OK, &rsp);

        }

        if (param->write.handle == char_handle) {
            //data written to char
            if (param->write.len > 0) {
                uint8_t v = param->write.value[0];
                gpio_set_level(LED, v ? 1: 0);
                printf("LED set to %d\n", v);

                //send notify if enabled and have valid connection
                if (notify_enabled && connected && gl_gatts_if != ESP_GATT_IF_NONE && gl_conn_id !=0) {
                uint8_t notify_data = v;
                esp_err_t ret = esp_ble_gatts_send_indicate(gl_gatts_if, gl_conn_id, char_handle, 1, &notify_data, false); //false notifycation not indication
                if (ret != ESP_OK) {
                    printf("Send notify failed : %s\n", esp_err_to_name(ret));
                } else {
                    printf("Notification sent succesfully\n");
                }
            }
        }
    } else if (param->write.handle == cccd_handle) {
        //cccd written -> enabled or disable notify
        if (param->write.len >= 2) {
            uint16_t cfg = param->write.value[0] | (param->write.value[1] << 8);
            notify_enabled = (cfg & 0x0001) ? true : false;
            printf("CCCD set to 0x%04x (notify %s)\n", cfg, notify_enabled ? "enabled" : "disabled");
        }
    }
    break;

    case ESP_GATTS_READ_EVT:
        printf("READ_EVT handle = %d\n", param->read.handle);
        if (param->read.handle == char_handle) {
            //return current led STate  
            esp_gatt_rsp_t rsp;
            memset(&rsp, 0, sizeof(esp_gatt_rsp_t));
            rsp.attr_value.handle = param->read.handle;
            rsp.attr_value.len = 1;
            rsp.attr_value.value[0] = gpio_get_level(LED);
            esp_ble_gatts_send_response(gatts_if, param->read.conn_id, param->read.trans_id, ESP_GATT_OK, &rsp);
        }
        break;
    default:
        break;
    }
}

void app_main(void) {
    esp_err_t ret;

    ret = nvs_flash_init();
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    gpio_reset_pin(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    gpio_set_level(LED, 0);

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BLE));

    ESP_ERROR_CHECK(esp_bluedroid_init());
    ESP_ERROR_CHECK(esp_bluedroid_enable());

    ESP_ERROR_CHECK(esp_ble_gap_register_callback(gap_cb));
    ESP_ERROR_CHECK(esp_ble_gatts_register_callback(gatts_cb));
    ESP_ERROR_CHECK(esp_ble_gatts_app_register(0));
    printf("BLE LED GATT Server Started...\n");
}
