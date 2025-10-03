#include <stdio.h>
#include "driver/adc.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s.h"

#define I2S_PORT I2S_NUM_0
#define SAMPLE_RATE 44100
#define READ_LEN 1024

void app_main(void)
{
    //cnfig i2s rx for adc internal
    i2s_config_t i2s_cfg = {
        .mode = I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_ADC_BUILT_IN,
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_MSB,
        .dma_buf_count = 4,
        .dma_buf_len = 256,
        .intr_alloc_flags = 0,
        .use_apll = false
    };
    
    ESP_ERROR_CHECK(i2s_driver_install(I2S_PORT, &i2s_cfg, 0, NULL));
    //attach adc to i2s 
    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_12));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_DB_12));
    ESP_ERROR_CHECK(i2s_set_adc_mode(ADC_UNIT_1, ADC1_CHANNEL_5));
    ESP_ERROR_CHECK(i2s_adc_enable(I2S_PORT));

    uint16_t buffer[READ_LEN];

    while (1) {
        size_t bytes_read= 0;
        // read data from i2s 
        esp_err_t ret = i2s_read(I2S_PORT, (void*) buffer, READ_LEN * sizeof(uint16_t), &bytes_read, portMAX_DELAY);

        if (ret == ESP_OK && bytes_read > 0) {
            printf("Read %d bytes\n", bytes_read);

            for (int i = 0; i < 10; i++) {
                printf("%d ", buffer[i] & 0xFFF);
            }
            printf("\n");
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
