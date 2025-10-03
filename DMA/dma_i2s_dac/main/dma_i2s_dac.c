#include <stdio.h>
#include <math.h>
#include "driver/i2s.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"

#define I2S_NUM I2S_NUM_0
#define SAMPLE_RATE 44100
#define PI 3.14159265

void app_main(void)
{
    //config i2s dac mde
    i2s_config_t i2s_cfg = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN,
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_MSB,
        .dma_buf_count = 4,
        .dma_buf_len = 1024,
        .intr_alloc_flags = 0,
        .tx_desc_auto_clear = true,
        .use_apll = false,
    };

    i2s_driver_install(I2S_NUM, &i2s_cfg, 0, NULL);
    i2s_set_dac_mode(I2S_DAC_CHANNEL_BOTH_EN);

    printf("I2S DAC Started, generating sine wave...\n");

    size_t bytes_written;
    short buffer[1024];

    while (1) {
        for (int i =0; i < 1024; i++) {
            buffer[i] = (short)(3000 * sin(2 * PI * 440 * i / SAMPLE_RATE));
        }
        i2s_write(I2S_NUM, buffer, sizeof(buffer), &bytes_written, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
