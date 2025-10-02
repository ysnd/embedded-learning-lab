#include <stdio.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/rmt.h"

#define RMT_RX_CH RMT_CHANNEL_0
#define RMT_RX_PIN 19
#define RMT_CLK_DIV 80
#define RMT_BUF_SIZE 2048


void app_main(void)
{
    //config rmt rx 
    rmt_config_t rx_cfg = {
        .channel = RMT_RX_CH,
        .gpio_num = RMT_RX_PIN,
        .clk_div = RMT_CLK_DIV,
        .mem_block_num = 4,
        .rmt_mode = RMT_MODE_RX,
        .rx_config.filter_en = true,
        .rx_config.filter_ticks_thresh = 100,
        .rx_config.idle_threshold = 10000,
    };

    ESP_ERROR_CHECK(rmt_config(&rx_cfg));
    ESP_ERROR_CHECK(rmt_driver_install(rx_cfg.channel, RMT_BUF_SIZE, 0));

    RingbufHandle_t rb = NULL;
    ESP_ERROR_CHECK(rmt_get_ringbuf_handle(rx_cfg.channel, &rb));
    rmt_rx_start(rx_cfg.channel, true);

    printf("RMT RX Started on gpio %d, waiting signal...\n", RMT_RX_PIN);
    
    while (1) {
        size_t rx_size = 0;
        rmt_item32_t *items=(rmt_item32_t *) xRingbufferReceive(rb, &rx_size, pdMS_TO_TICKS(1000));
        int num_items = rx_size / sizeof(rmt_item32_t);
        printf("Captured %d RMT items:\n", num_items);

        for (int i = 0; i < num_items; i++) {
            uint32_t high_us = items[i].duration0;
            uint32_t low_us = items[i].duration1;
            int level0 = items[i].level0;
            int level1 = items[i].level1;

            printf("Item %02d: level0=%d dur0=%ldus | level1=%d dur1=%ldus\n",i , level0, high_us, level1, low_us);

        }
        vRingbufferReturnItem(rb, (void*) items);
    }

}
