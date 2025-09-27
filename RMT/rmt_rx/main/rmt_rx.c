#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/rmt.h"
#include "esp_log.h"

#define RMT_RX_CH RMT_CHANNEL_0
#define RMT_RX_PIN 19
#define RMT_CLK_DIV 80
#define RMT_RX_BUFFER_SIZE 1024

void app_main(void) {
    rmt_config_t rmt_rx;
    rmt_rx.channel = RMT_RX_CH;
    rmt_rx.gpio_num = RMT_RX_PIN;
    rmt_rx.clk_div = RMT_CLK_DIV;
    rmt_rx.mem_block_num = 1;
    rmt_rx.rmt_mode = RMT_MODE_RX;

    rmt_rx.rx_config.filter_en = true;
    rmt_rx.rx_config.filter_ticks_thresh = 50;
    rmt_rx.rx_config.idle_threshold = 20000;

    ESP_ERROR_CHECK(rmt_config(&rmt_rx));
    ESP_ERROR_CHECK(rmt_driver_install(rmt_rx.channel, RMT_RX_BUFFER_SIZE, 0));

    RingbufHandle_t rb = NULL;
    ESP_ERROR_CHECK(rmt_get_ringbuf_handle(rmt_rx.channel, &rb));
    rmt_rx_start(rmt_rx.channel, true);

    printf("RMT RX Started, ready to receive NEC remote\n");

    while (1) {
        size_t rx_size = 0;
        rmt_item32_t* items = (rmt_item32_t*) xRingbufferReceive(rb, &rx_size, pdMS_TO_TICKS(1000));
        if (items) {
            int num_items = rx_size / sizeof(rmt_item32_t);
            printf("Received %d RMT items\n", num_items);

            //contoh print level diuration
            for (int i = 0; i < num_items; i++) {
                printf("[%d] H:%d %dus L:%d %dus\n", i, items[i].level0, items[i].duration0,
                                                items[i].level1, items[i].duration1);
            }
            vRingbufferReturnItem(rb, (void*)items);
        }
    }
}
