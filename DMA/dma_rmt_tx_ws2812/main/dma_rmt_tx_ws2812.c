#include <stdio.h>
#include "driver/rmt_encoder.h"
#include "driver/rmt_tx.h"
#include "esp_cpu.h"
#include "esp_err.h"

#define LED_PIN 18
#define LED_NUM 8 //jumlah led  ws2812

uint8_t led_data[LED_NUM * 3] = {
    0x10, 0x00, 0x00, // LED1 RED
    0x00, 0x10, 0x00, // LED2 GREEN
    0x00, 0x00, 0x10, // LED3 BLUE
    0x10, 0x10, 0x00, // LED4 YELLOW
    0x00, 0x10, 0x10, // LED5 CYAN
    0x10, 0x00, 0x10, // LED6 MAGENTA
    0x08, 0x08, 0x08, // LED7 WHITE DIM
    0x00, 0x00, 0x00, // LED8 OFF
};


void app_main(void)
{
    printf("Init RMT TX Channel...\n");
    //create tx Channel
    rmt_tx_channel_config_t tx_ch_cfg = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .gpio_num = LED_PIN,
        .mem_block_symbols = 64,
        .resolution_hz = 10 * 1000 * 1000,
        .trans_queue_depth = 4,
    };
    rmt_channel_handle_t tx_ch = NULL;
    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_ch_cfg, &tx_ch));

    //creeeate encder for ws2812
    rmt_encoder_handle_t led_enc = NULL;
    rmt_copy_encoder_config_t encoder_config = {};
    ESP_ERROR_CHECK(rmt_new_copy_encoder(&encoder_config, &led_enc));
    // enable Channel
    ESP_ERROR_CHECK(rmt_enable(tx_ch));
    printf("Kirim data LED via DMA...\n");
    //transmit data
    rmt_transmit_config_t tx_cfg = {
        .loop_count = 0,
    };
    ESP_ERROR_CHECK(rmt_transmit(tx_ch, led_enc, led_data, sizeof(led_data), &tx_cfg));
    //wait until the end    
    ESP_ERROR_CHECK(rmt_tx_wait_all_done(tx_ch, -1));
}
