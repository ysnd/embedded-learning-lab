#include <stdio.h>
#include "driver/rmt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define RMT_TX_CH RMT_CHANNEL_0
#define RMT_TX_GPIO 18


void app_main(void)
{
    printf("Initialize RMT TX on GPIO%d\n", RMT_TX_GPIO);

    //config legacy RMT 
    rmt_config_t cfg = {
        .rmt_mode = RMT_MODE_TX,
        .channel = RMT_TX_CH,
        .gpio_num = RMT_TX_GPIO,
        .clk_div = 80, //1 tick = 1us
        .mem_block_num = 1,
        .tx_config.loop_en = false
    };
    rmt_config(&cfg);
    rmt_driver_install(cfg.channel, 0, 0);

    rmt_item32_t waveform_100hz_50 = {
        .level0 =1, .duration0 = 5000, //50%duty
        .level1 =0, .duration1 = 5000
    };

    rmt_item32_t waveform_75hz_50 = {
        .level0 =1, .duration0 = 6666,
        .level1 =0, .duration1 = 6666
    };

    rmt_item32_t waveform_100hz_25 = {
        .level0 =1, .duration0 = 2500,
        .level1 =0, .duration1 = 7500
    };

    rmt_item32_t waveform_100hz_75 = {
        .level0 =1, .duration0 = 7500,
        .level1 =0, .duration1 = 2500
    };

    while (1) {
        printf("LED 100Hz, duty 50%%\n");
        ESP_ERROR_CHECK(rmt_write_items(RMT_TX_CH, &waveform_100hz_50, 1, true));
        vTaskDelay(pdMS_TO_TICKS(3000));
        printf("LED 75Hz, duty 50%%\n");
        ESP_ERROR_CHECK(rmt_write_items(RMT_TX_CH, &waveform_75hz_50, 1, true));
        vTaskDelay(pdMS_TO_TICKS(3000));
         printf("LED 100Hz, duty 25%%\n");
        ESP_ERROR_CHECK(rmt_write_items(RMT_TX_CH, &waveform_100hz_25, 1, true));
        vTaskDelay(pdMS_TO_TICKS(3000));
         printf("LED 100Hz, duty 75%%\n");
        ESP_ERROR_CHECK(rmt_write_items(RMT_TX_CH, &waveform_100hz_75, 1, true));
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}
    

