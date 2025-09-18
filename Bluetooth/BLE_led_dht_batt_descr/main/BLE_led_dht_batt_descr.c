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
#include "dht11.h"
#include "esp_bt_device.h"
#include "esp_bt_defs.h"
#include "driver/adc.h"


#define LED 2
#define DHT 17
#define BATT ADC1_CHANNEL_5

//uuid
#define SERVICE_UUID 0x00FF
#define CHAR_UUID_LED 0xFF01
#define CHAR_UUID_DHT 0xFF02
#define BATTERY_SERVICE_UUID 0x180F
#define BATTERY_LEVEL_CHAR_UUID 0x2A19

static esp_ble_adv_params_t adv_params = {
    .adv_int_min = 0x20,
    .adv_int_max = 0x40,
    .adv_type = ADV_TYPE_IND,
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
    .channel_map = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY
};

static uint16_t gl_service_handle = 0;
static esp_gatt_if_t gl_gatts_if = 0;
static uint16_t gl_conn_id = 0;
static bool gl_connected = false;

static uint16_t gl_led_value_handle = 0;
static uint16_t gl_led_cccd_handle = 0;
static bool gl_led_notify_en = false;

static uint16_t gl_dht_value_handle = 0;
static uint16_t gl_dht_cccd_handle = 0;
static bool gl_dht_notify_en = false;

static bool led_char_added = false;
static bool led_cccd_added = false;

static uint16_t gl_batt_service_handle = 0;
static uint16_t gl_batt_value_handle = 0;
static uint16_t gl_batt_cccd_handle = 0;
static bool gl_batt_notify_en = false;
static bool batt_service_added = false;
static bool batt_char_added = false;
static bool batt_cccd_added = false;

static bool led_user_desc_added = false;
static bool dht_user_desc_added = false;
static bool battery_user_desc_added = false;

static void adc_init(void) {
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(BATT, ADC_ATTEN_DB_12);
    printf("ADC initialized for battery reading on GPIO33\n");
}

static uint8_t read_batt_level(void) {
    int raw = adc1_get_raw(BATT);
    uint8_t battery_percentage = (raw * 100) / 4095;
    return battery_percentage;
}


static void ble_security_init(void) {
    //set auth = bunding +MITM optional
    esp_ble_auth_req_t auth_req = ESP_LE_AUTH_REQ_SC_BOND;
    esp_ble_io_cap_t iocap = ESP_IO_CAP_IO;
    uint8_t oob_support = ESP_BLE_OOB_DISABLE;
    uint8_t key_size = 16;
    uint8_t init_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
    uint8_t rsp_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
    uint32_t passkey = 123456;

    esp_ble_gap_set_security_param(ESP_BLE_SM_AUTHEN_REQ_MODE, &auth_req, sizeof(auth_req));
    esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE, &iocap, sizeof(iocap));
    esp_ble_gap_set_security_param(ESP_BLE_SM_OOB_SUPPORT, &oob_support, sizeof(oob_support));
    esp_ble_gap_set_security_param(ESP_BLE_SM_MAX_KEY_SIZE, &key_size, sizeof(key_size));
    esp_ble_gap_set_security_param(ESP_BLE_SM_SET_INIT_KEY, &init_key, sizeof(init_key));
    esp_ble_gap_set_security_param(ESP_BLE_SM_SET_RSP_KEY, &rsp_key, sizeof(rsp_key));
    esp_ble_gap_set_security_param(ESP_BLE_SM_SET_STATIC_PASSKEY, &passkey, sizeof(passkey));
    
    printf("BLE security param set (Bounding + MITM + static passkey\n");
}

static void make_uuid16(esp_bt_uuid_t *u, uint16_t u16) {
    u->len = ESP_UUID_LEN_16;
    u->uuid.uuid16 = u16;
}

static uint8_t adv_service_uuid128[16] = {  
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x00, (SERVICE_UUID & 0xFF), (SERVICE_UUID >> 8), 0x00, 0x00  
};

//gap callback
static void gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
    switch (event) {
    case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
        printf("adv data set complete -> start adv\n");
        esp_ble_gap_start_advertising(&adv_params);
        break;

    case ESP_GAP_BLE_SEC_REQ_EVT:
        //peer meminta security
        printf("ESP_GAP_BLE_SEC_REQ_EVT: peer security request\n");
        esp_ble_gap_security_rsp(param->ble_security.ble_req.bd_addr, true);
        break;

    case ESP_GAP_BLE_AUTH_CMPL_EVT:
        if (param->ble_security.auth_cmpl.success) {
            printf("ESP_GAP_BLE_AUTH_CMPL_EVT: auth success\n");
        } else {
            printf("ESP_GAP_BLE_AUTH_CMPL_EVT: auth failed reason=0x%02x\n", param->ble_security.auth_cmpl.fail_reason);
        }
        break;

    case ESP_GAP_BLE_PASSKEY_REQ_EVT:
        printf("PASSKEY_REQ_EVT: need to input passkey\n");
        break;

    case ESP_GAP_BLE_NC_REQ_EVT:
        printf("NUMERIC_COMPARSION_REQ_EVT: passkey=%ld\n", param->ble_security.key_notif.passkey);
        esp_ble_confirm_reply(param->ble_security.key_notif.bd_addr, true);
        break;

    default:
        break;
    }
}

static void send_notify_uint8(esp_gatt_if_t gatts_if, uint16_t conn_id, uint16_t value_handle, uint8_t val) {
    if (conn_id == 0) return;
    esp_err_t err = esp_ble_gatts_send_indicate(gatts_if, conn_id, value_handle, sizeof(val), &val, false);
    printf("send indicate(handle=%d) - %s\n", value_handle, esp_err_to_name(err));
}

static void sensor_task(void *arg) {
    dht11_data_t d;
    uint8_t last_battery = 0;
    while (1) {
        if (gl_connected && gl_dht_notify_en && gl_dht_value_handle !=0) {
            if (dht11_read(&d)) {
                // pack temperature & humidity as two int16 (x10) little endian
                int16_t t10 = (int16_t)(d.temperature * 10.0);
                int16_t h10 = (int16_t)(d.humidity * 10.0);
                uint8_t payload[4];
                payload[0] = t10 & 0xFF;
                payload[1] = (t10 >> 8) & 0xFF;
                payload[2] = h10 & 0xFF;
                payload[3] = (h10 >> 8) & 0xFF;
                esp_err_t e = esp_ble_gatts_send_indicate(gl_gatts_if, gl_conn_id, gl_dht_value_handle, sizeof(payload), payload, false);
                printf("DHT notify T = %.1f H=%.1f -> send indicate : %s\n", d.temperature, d.humidity, esp_err_to_name(e));
            } else {
                printf("DHT Read failed\n");
            }
        }
        if (gl_connected && gl_batt_notify_en && gl_batt_service_handle != 0) {
            uint8_t battery_level = read_batt_level();
            if (abs(battery_level - last_battery) >= 5) {
                esp_err_t e= esp_ble_gatts_send_indicate(gl_gatts_if, gl_conn_id, gl_batt_value_handle, sizeof(battery_level), &battery_level, false);
                printf("Battery notify: %d%% -> %s\n", battery_level, esp_err_to_name(e));
                last_battery = battery_level;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(2000));
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
            
            esp_ble_adv_data_t adv_data = {
                .set_scan_rsp = false,
                .include_name = true,
                .include_txpower = true,
                .service_uuid_len = sizeof(adv_service_uuid128),
                .p_service_uuid = adv_service_uuid128,
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

            err = esp_ble_gatts_create_service(gatts_if, &srvc_id, 8);
            if (err != ESP_OK) {
                printf("create service failed : %s\n", esp_err_to_name(err));
            }
        }
        break;

    case ESP_GATTS_CREATE_EVT:
        printf("CREATE_EVT, status %d, service_handle %d\n", param->create.status, param->create.service_handle);
        if (gl_service_handle == 0) {
            gl_service_handle = param->create.service_handle;

        //start service
            esp_ble_gatts_start_service(gl_service_handle);

        //add caracteristic (read/write/notify)
                esp_bt_uuid_t char_uuid;
                make_uuid16(&char_uuid, CHAR_UUID_LED);
                esp_attr_value_t char_val = {
                    .attr_max_len = 1,
                    .attr_len = 1,
                    .attr_value = (uint8_t[]){0x00}
                };                    
                err = esp_ble_gatts_add_char(gl_service_handle, &char_uuid, ESP_GATT_PERM_READ_ENC_MITM | ESP_GATT_PERM_WRITE_ENC_MITM , ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_NOTIFY, &char_val, NULL);
                printf("add char LED -> %s\n", esp_err_to_name(err));
        } else if (gl_batt_service_handle == 0) {
            gl_batt_service_handle = param->create.service_handle;
            esp_ble_gatts_start_service(gl_batt_service_handle);

            esp_bt_uuid_t battery_char_uuid;
            make_uuid16(&battery_char_uuid, BATTERY_LEVEL_CHAR_UUID);
            esp_attr_value_t battery_char_val = {
                .attr_max_len = 1,
                .attr_len = 1,
                .attr_value = (uint8_t[]){50}
            };
            err = esp_ble_gatts_add_char(gl_batt_service_handle, &battery_char_uuid, ESP_GATT_PERM_READ, ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY, &battery_char_val, NULL);
                printf("add battery level char -> %s\n", esp_err_to_name(err));
        }
        break;

    case ESP_GATTS_ADD_CHAR_EVT:
        printf("ADD_CHAR_EVT, attr_handle %d uuid=0x%04x\n", param->add_char.attr_handle, (param->add_char.char_uuid.len == ESP_UUID_LEN_16) ? param->add_char.char_uuid.uuid.uuid16 : 0);
        if (param->add_char.char_uuid.len == ESP_UUID_LEN_16) {
            uint16_t u = param->add_char.char_uuid.uuid.uuid16;
            if (u == CHAR_UUID_LED) {
                gl_led_value_handle = param->add_char.attr_handle;
                led_char_added = true;
                printf("LED Value handle = %d\n", gl_led_value_handle);
            } else if (u == CHAR_UUID_DHT) {
                gl_dht_value_handle = param->add_char.attr_handle;
                printf("DHT Value handle = %d\n", gl_dht_value_handle);
            } else if (u == BATTERY_LEVEL_CHAR_UUID) {
                gl_batt_value_handle = param->add_char.attr_handle;
                batt_char_added=true;
                printf("Battery value handle = %d\n", gl_batt_value_handle);
            } else {
                printf("unknown char_uudi\n");
            }
        }
        //add user description
        {
            esp_bt_uuid_t user_descr_uuid;
            make_uuid16(&user_descr_uuid, ESP_GATT_UUID_CHAR_DESCRIPTION); //0x2902    
            
            const char* description = "";
            if (param->add_char.char_uuid.len == ESP_UUID_LEN_16) {
                uint16_t u = param->add_char.char_uuid.uuid.uuid16;
                if (u == CHAR_UUID_LED) {
                    description = "LED Control - Write 1 to turn on, 0 to turn off";
                } else if (u == CHAR_UUID_DHT) {
                    description = "DHT11 Sensor - Temperature(C*10) + Humidity(%*10) in 4 bytes LE";
                } else if (u == BATTERY_LEVEL_CHAR_UUID) {
                    description = "Battery Level - Percentage (0-100%)";
                }
            }
            uint16_t service_handle = gl_service_handle;
            if (param->add_char.char_uuid.len == ESP_UUID_LEN_16 && param->add_char.char_uuid.uuid.uuid16 == BATTERY_LEVEL_CHAR_UUID) {
                service_handle = gl_batt_service_handle;
            }

            esp_attr_value_t user_descr_val = {
                .attr_max_len = strlen(description),
                .attr_len = strlen(description),
                .attr_value = (uint8_t*)description
            };
            err = esp_ble_gatts_add_char_descr(service_handle, &user_descr_uuid, ESP_GATT_PERM_READ, &user_descr_val, NULL);
            if (err != ESP_OK) {
                printf("add_user description failed : %s\n", esp_err_to_name(err));
            } else {
                printf("added user description:\"%s\"\n", description);
            }
        }
        break;

    case ESP_GATTS_ADD_CHAR_DESCR_EVT:
		printf("ADD_DESCR_EVT, handle %d\n", param->add_char_descr.attr_handle);
    
    // Track based on order: User Description added first, then CCCD
		if (led_char_added && !led_user_desc_added) {
			led_user_desc_added = true;
			printf("LED user description added\n");
        
        // Now add CCCD descriptor for LED
			esp_bt_uuid_t cccd_uuid;
			make_uuid16(&cccd_uuid, ESP_GATT_UUID_CHAR_CLIENT_CONFIG);
			esp_attr_value_t cccd_val = {
				.attr_max_len = 2,
				.attr_len = 2,
				.attr_value = (uint8_t[]){0x00, 0x00}
			};
			err = esp_ble_gatts_add_char_descr(gl_service_handle, &cccd_uuid, ESP_GATT_PERM_READ_ENC_MITM | ESP_GATT_PERM_WRITE_ENC_MITM, &cccd_val, NULL);
			printf("adding LED CCCD: %s\n", esp_err_to_name(err));
        
		} else if (led_char_added && led_user_desc_added && !led_cccd_added) {
			gl_led_cccd_handle = param->add_char_descr.attr_handle;
			led_cccd_added = true;
			printf("mapped led_cccd_handle = %d\n", gl_led_cccd_handle);
        
        // Add DHT characteristic after LED complete
			esp_bt_uuid_t char_uuid;
			make_uuid16(&char_uuid, CHAR_UUID_DHT);
			esp_attr_value_t char_val = {
				.attr_max_len = 4,
				.attr_len = 4,
				.attr_value = (uint8_t[]){0x00, 0x00, 0x00, 0x00}
			};
			err = esp_ble_gatts_add_char(gl_service_handle, &char_uuid, ESP_GATT_PERM_READ, ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY, &char_val, NULL);
			printf("add char DHT -> %s\n", esp_err_to_name(err));
        
		} else if (gl_dht_value_handle != 0 && !dht_user_desc_added) {
			dht_user_desc_added = true;
			printf("DHT user description added\n");
        
        // Add CCCD for DHT
			esp_bt_uuid_t cccd_uuid;
			make_uuid16(&cccd_uuid, ESP_GATT_UUID_CHAR_CLIENT_CONFIG);
			esp_attr_value_t cccd_val = {
				.attr_max_len = 2,
				.attr_len = 2,
				.attr_value = (uint8_t[]){0x00, 0x00}
			};
			err = esp_ble_gatts_add_char_descr(gl_service_handle, &cccd_uuid, ESP_GATT_PERM_READ_ENC_MITM | ESP_GATT_PERM_WRITE_ENC_MITM, &cccd_val, NULL);
			printf("adding DHT CCCD: %s\n", esp_err_to_name(err));
        
		} else if (gl_dht_value_handle != 0 && dht_user_desc_added && gl_dht_cccd_handle == 0) {
			gl_dht_cccd_handle = param->add_char_descr.attr_handle;
			printf("mapped dht_cccd_handle = %d\n", gl_dht_cccd_handle);
        
		} else if (batt_char_added && !battery_user_desc_added) {
			battery_user_desc_added = true;
			printf("Battery user description added\n");
        
        // Add CCCD for Battery
			esp_bt_uuid_t cccd_uuid;
			make_uuid16(&cccd_uuid, ESP_GATT_UUID_CHAR_CLIENT_CONFIG);
			esp_attr_value_t cccd_val = {
				.attr_max_len = 2,
				.attr_len = 2,
				.attr_value = (uint8_t[]){0x00, 0x00}
			};
			err = esp_ble_gatts_add_char_descr(gl_batt_service_handle, &cccd_uuid, ESP_GATT_PERM_READ_ENC_MITM | ESP_GATT_PERM_WRITE_ENC_MITM, &cccd_val, NULL);
			printf("adding Battery CCCD: %s\n", esp_err_to_name(err));
        
		} else if (batt_char_added && battery_user_desc_added && !batt_cccd_added) {
			gl_batt_cccd_handle = param->add_char_descr.attr_handle;
			batt_cccd_added = true;
			printf("mapped battery_cccd_handle = %d\n", gl_batt_cccd_handle);
        
		} else {
			printf("descriptor mapping ambiguous / ignored\n");
		}
		break;

    case ESP_GATTS_START_EVT:
        printf("SERVICE START_EVT\n");

        if (!batt_service_added) {
            esp_gatt_srvc_id_t battery_srvc_id;
            memset(&battery_srvc_id, 0, sizeof(battery_srvc_id));
            battery_srvc_id.is_primary = true;
            battery_srvc_id.id.inst_id = 0;
            make_uuid16(&battery_srvc_id.id.uuid, BATTERY_SERVICE_UUID);
            err = esp_ble_gatts_create_service(gatts_if, &battery_srvc_id, 4);
            if (err == ESP_OK) {
                batt_service_added = true;
                printf("Battery service creation initialized\n");
            } else {
                printf("Battery service creation failed: %s\n", esp_err_to_name(err));
            }
        }
        break;

    case ESP_GATTS_CONNECT_EVT:
        printf("CONNECT_EVT, conn_id %d\n", param->connect.conn_id);
        gl_gatts_if = gatts_if;
        gl_conn_id = param->connect.conn_id;
        gl_connected = true;
        gl_led_notify_en = false; //reset notify new conection
        gl_dht_notify_en = false;
		gl_batt_notify_en = false;
        esp_ble_set_encryption(param->connect.remote_bda, ESP_BLE_SEC_ENCRYPT_MITM);
        break;

    case ESP_GATTS_DISCONNECT_EVT:
        printf("Disconect reason = 0x%04x\n", param->disconnect.reason);
        gl_connected = false;
        gl_dht_notify_en = false;
        gl_led_notify_en = false;
        gl_batt_notify_en = false;
        gl_conn_id = 0;
        //restart adv   
        esp_ble_gap_start_advertising(&adv_params);
        break;

    case ESP_GATTS_WRITE_EVT:
        printf("WRITE_EVT handle=%d, len=%d need_rsp=%d\n", param->write.handle, param->write.len, param->write.need_rsp);
        //send write response first
        if (param->write.need_rsp) {
            esp_gatt_rsp_t rsp;
            memset(&rsp, 0, sizeof(rsp));
            rsp.attr_value.handle = param->write.handle;
            rsp.attr_value.len = (param-> write.len <= sizeof(rsp.attr_value.value)) ? param->write.len : sizeof(rsp.attr_value.value);
            if (param->write.len > 0) memcpy(rsp.attr_value.value, param->write.value, rsp.attr_value.len);
            esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, ESP_GATT_OK, &rsp);
            printf("Sent write response\n");            
        }

        if (param->write.handle == gl_led_cccd_handle) {
            //data written to char
            if (param->write.len >= 2) {
                uint8_t cfg = param->write.value[0] | (param->write.value[1] << 8);
                gl_led_notify_en = (cfg & 0x0001) ? true : false;
                printf("LED cccd set to 0x%04x -> notify=%d\n", cfg, gl_led_notify_en);
            }
            break;
        } else if (param->write.handle == gl_dht_cccd_handle) {
            if (param->write.len >= 2) {
                uint16_t cfg = param->write.value[0] | (param->write.value[1] << 8);
                gl_dht_notify_en = (cfg & 0x0001) ? true : false;
                printf("DHT CCCD set to 0x%04x -> notify=%d\n", cfg, gl_dht_notify_en);
            }
            break;
        } else if (param->write.handle == gl_batt_cccd_handle) {
            if (param->write.len >= 2) {
                uint16_t cfg = param->write.value[0] | (param->write.value[1] << 8);
                gl_batt_notify_en = (cfg & 0x0001) ? true : false;
                printf("Battery CCCD set to 0x%04x -> notify=%d\n", cfg, gl_batt_notify_en);
            }
            break;
        }
        if (param->write.handle == gl_led_value_handle) {
            if (param->write.len >= 1) {
                uint8_t v = param->write.value[0];
                gpio_set_level(LED, v ? 1 : 0);
                printf("LED set to %d\n", v);
                // send notify to client if enabled
                if (gl_connected && gl_led_notify_en && gl_led_value_handle != 0) {
                    send_notify_uint8(gl_gatts_if, gl_conn_id, gl_led_value_handle, v);
                }
            }
            break;
        }
        break;

    case ESP_GATTS_READ_EVT:
        printf("READ_EVT handle = %d\n", param->read.handle);
        if (param->read.handle == gl_led_value_handle) {
            //return current led STate  
            esp_gatt_rsp_t rsp;
            memset(&rsp, 0, sizeof(esp_gatt_rsp_t));
            rsp.attr_value.handle = param->read.handle;
            rsp.attr_value.len = 1;
            rsp.attr_value.value[0] = gpio_get_level(LED);
            esp_ble_gatts_send_response(gatts_if, param->read.conn_id, param->read.trans_id, ESP_GATT_OK, &rsp);
            printf("Replied LED read = %d", rsp.attr_value.value[0]);
        } else if (param->read.handle == gl_dht_value_handle) {
            // read DHT & respond with 4 bytes (temp*10 little endian, hum*10 little endian)
            dht11_data_t d;
            esp_gatt_rsp_t rsp;
            memset(&rsp, 0, sizeof(rsp));
            rsp.attr_value.handle = param->read.handle;
            rsp.attr_value.len = 4;
            if (dht11_read(&d)) {
                int16_t t10 = (int16_t)(d.temperature * 10.0);
                int16_t h10 = (int16_t)(d.humidity * 10.0);
                rsp.attr_value.value[0] = t10 & 0xFF;
                rsp.attr_value.value[1] = (t10 >> 8) & 0xFF;
                rsp.attr_value.value[2] = h10 & 0xFF;
                rsp.attr_value.value[3] = (h10 >> 8) & 0xFF;
                printf("DHT read for READ_EVT: T=%.1f H=%.1f\n", d.temperature, d.humidity);
            } else {
                rsp.attr_value.value[0] = rsp.attr_value.value[1] = rsp.attr_value.value[2] = rsp.attr_value.value[3] = 0xFF;
                printf("DHT read failed for READ_EVT\n");
            }
            esp_ble_gatts_send_response(gatts_if, param->read.conn_id, param->read.trans_id, ESP_GATT_OK, &rsp);
        } else if (param->read.handle == gl_batt_value_handle) {
            esp_gatt_rsp_t rsp;
            memset(&rsp, 0, sizeof(rsp));
            rsp.attr_value.handle = param->read.handle;
            rsp.attr_value.len = 1;
            rsp.attr_value.value[0] = read_batt_level();
            esp_ble_gatts_send_response(gatts_if, param->read.conn_id, param->read.trans_id, ESP_GATT_OK, &rsp);
            printf("Battery read response:%d%%\n", rsp.attr_value.value[0]);
        }
        break;

    case ESP_GATTS_CONF_EVT:
        printf("GATTS_CONF_EVT status:%d handle:%d\n", param->conf.status, param->conf.handle);        

    default:
        break;
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

    gpio_reset_pin(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    gpio_set_level(LED, 0);

    dht11_init(DHT);
    adc_init();

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BLE));

    ESP_ERROR_CHECK(esp_bluedroid_init());
    ESP_ERROR_CHECK(esp_bluedroid_enable());

    ble_security_init();

    ESP_ERROR_CHECK(esp_ble_gap_register_callback(gap_cb));
    ESP_ERROR_CHECK(esp_ble_gatts_register_callback(gatts_cb));
    ESP_ERROR_CHECK(esp_ble_gatts_app_register(0));

    xTaskCreate(sensor_task, "sensor_task", 4096, NULL, 5, NULL);
    printf("BLE LED+DHT GATT Server Started...\n");
}

