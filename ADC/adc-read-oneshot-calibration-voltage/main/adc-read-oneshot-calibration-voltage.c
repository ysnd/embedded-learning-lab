#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali_scheme.h"

static char const *TAG = "ESP ADC ONESHOT AND CALI";

static adc_oneshot_unit_handle_t adc1_handle;
static adc_cali_handle_t adc1_cali;
static bool adc_calibrated = false;

void adc_init(void) {
    adc_oneshot_unit_init_cfg_t init_cfg = {
        .unit_id = ADC_UNIT_1,
    };

    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_cfg, &adc1_handle));

    adc_oneshot_chan_cfg_t config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };

    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_6, &config));

    adc_cali_line_fitting_config_t cali_cfg = {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };

    esp_err_t ret = adc_cali_create_scheme_line_fitting(&cali_cfg, &adc1_cali);
    if (ret == ESP_OK) {
        adc_calibrated = true;
        ESP_LOGI(TAG, "ADC kalibrasi enable");
    } else {
        ESP_LOGW(TAG, "ADC kalibrasi unavailable (%s)", esp_err_to_name(ret));
    }
}

void app_main(void)
{
    adc_init();
    while (1) {
        int raw, mv;
        adc_oneshot_read(adc1_handle, ADC_CHANNEL_6, &raw);
        if (adc_calibrated) {
            adc_cali_raw_to_voltage(adc1_cali, raw, &mv);
        } else {
            mv = raw;
        }
        ESP_LOGI(TAG, "RAW = %d", raw);
        ESP_LOGI(TAG, "Voltage mv = %d", mv);
        vTaskDelay(pdMS_TO_TICKS(200));
    
    }
}

