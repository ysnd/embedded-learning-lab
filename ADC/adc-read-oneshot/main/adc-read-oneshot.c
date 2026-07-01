#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"

static char *TAG = "ESP ADC ONESHOT";

static adc_oneshot_unit_handle_t adc1_handle;

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
}

void app_main(void)
{
    adc_init();
    while (1) {
        int raw;
        adc_oneshot_read(adc1_handle, ADC_CHANNEL_6, &raw);
        ESP_LOGI(TAG, "RAW = %d", raw);
        vTaskDelay(pdMS_TO_TICKS(200));
    
    }
}

