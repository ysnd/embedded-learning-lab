#include <stdbool.h>
#include <stdio.h>
#include "driver/rmt.h"
#include "driver/rmt_types_legacy.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/projdefs.h"

#define RMT_TX_CH RMT_CHANNEL_0
#define RMT_TX_GPIO 18
#define RMT_CLK_DIV 80 // 1 tick = 1us (80Mhz/80 = 1MHz)

void app_main(void) {
    rmt_config_t rmt_tx;
    rmt_tx.channel = RMT_TX_CH;
    rmt_tx.gpio_num = RMT_TX_GPIO;
    rmt_tx.clk_div = RMT_CLK_DIV;
    rmt_tx.mem_block_num = 1;
    rmt_tx.rmt_mode = RMT_MODE_TX;
    rmt_tx.tx_config.loop_en = false;
    rmt_tx.tx_config.carrier_en = false; //disable IR carrier
    rmt_tx.tx_config.idle_output_en = true;
    rmt_tx.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;

    ESP_ERROR_CHECK(rmt_config(&rmt_tx));
    ESP_ERROR_CHECK(rmt_driver_install(rmt_tx.channel, 0, 0));


    //make simple waveform : 1000us, low 1000us, repeat
    rmt_item32_t items[2];
    items[0].level0 = 1; //high
    items[0].duration0 = 1000;//1000us
    items[0].level1 = 0;//low
    items[0].duration1 = 1000;//1000us

    items[1].level0 = 1;
    items[1].duration0 = 2000; //high 2000us
    items[1].level1 = 0;
    items[1].duration1 = 2000; //low 2000us

    printf("Mulai transmt pola RMT...");

    while (1) {
        ESP_ERROR_CHECK(rmt_write_items(RMT_TX_CH, items, 2, true));
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

