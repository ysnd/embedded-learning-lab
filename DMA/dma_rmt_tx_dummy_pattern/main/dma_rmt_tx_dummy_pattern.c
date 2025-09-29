#include <stdio.h>
#include "driver/rmt_tx.h"
#include "driver/rmt_encoder.h"
#include "esp_err.h"


#define RMT_TX_PIN 18

rmt_symbol_word_t dummy_pattern[10];

void app_main(void)
{
    printf("Init RMT TX channel...\n");
    //isi pola high 1000us, low 1000us, berulang
    for (int i = 0; i < 5; i++) {
        dummy_pattern[i].duration0 = 1000;
        dummy_pattern[i].level0 = 1;
        dummy_pattern[i].duration1 = 1000;
        dummy_pattern[i].level1 = 0;
    }
    //create tx channel
    rmt_tx_channel_config_t tx_ch_cfg = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .gpio_num = RMT_TX_PIN,
        .mem_block_symbols = 64,
        .resolution_hz = 1 * 1000 *1000,
        .trans_queue_depth = 4,
    };
    rmt_channel_handle_t tx_chan = NULL;
    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_ch_cfg, &tx_chan));

    //create encodere copy (direct cpy buffer -> rmt)
    rmt_encoder_handle_t cpy_encoder = NULL;
    rmt_copy_encoder_config_t cpy_cfg = {};
    ESP_ERROR_CHECK(rmt_new_copy_encoder(&cpy_cfg, &cpy_encoder));
    //enablee channel
    ESP_ERROR_CHECK(rmt_enable(tx_chan));
    //send data
    printf("Transmit dummy pattern via DMA...\n");
    rmt_transmit_config_t tx_cfg = {
        .loop_count = 0, //send once
    };
    ESP_ERROR_CHECK(rmt_transmit(tx_chan, cpy_encoder, dummy_pattern, sizeof(dummy_pattern), &tx_cfg));
    ESP_ERROR_CHECK(rmt_tx_wait_all_done(tx_chan, -1));
    printf("Selesai transmit pola dummy!\n");
}
