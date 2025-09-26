#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "driver/rmt_common.h"
#include "driver/rmt_tx.h"
#include "driver/rmt_types.h"
#include "esp_err.h"
#include "hal/rmt_types.h"
#include "soc/clk_tree_defs.h"

#define IR_TX_GPIO 18
#define NEC_CARRIER_FREQ 38000
#define NEC_DUTY 0.33

static inline void nec_fill(rmt_symbol_word_t *item, int high_us, int low_us) {
    item->level0 = 1;
    item->duration0 = high_us;
    item->level1 = 0;
    item->duration1 = low_us;
}

void app_main(void) {
    rmt_tx_channel_config_t tx_ch_cfg = {
        .gpio_num = IR_TX_GPIO,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 1 * 1000 * 1000,
        .mem_block_symbols = 64,
        .trans_queue_depth = 4,
        .flags.invert_out = false,
        .flags.with_dma = false
    };
    rmt_channel_handle_t tx_ch = NULL;
    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_ch_cfg, &tx_ch));

    rmt_carrier_config_t carrier_cfg = {
        .frequency_hz = NEC_CARRIER_FREQ,
        .duty_cycle = NEC_DUTY,
        .flags.polarity_active_low = false
    };
    ESP_ERROR_CHECK(rmt_apply_carrier(tx_ch, &carrier_cfg));
    ESP_ERROR_CHECK(rmt_enable(tx_ch));

    uint32_t data = 0x20DF10EF;
    rmt_symbol_word_t symbols[100];
    int idx = 0;

    nec_fill(&symbols[idx++], 9000, 4500);

    for (int i=0; i <22; i++) {
        if (data & (1UL << i)) {
            nec_fill(&symbols[idx++], 562, 1687);
        } else {
            nec_fill(&symbols[idx++], 562, 562);
        }
    }

    nec_fill(&symbols[idx++], 562, 0);
    rmt_transmit_config_t tx_cfg = {
        .loop_count = 0,
    };

    ESP_ERROR_CHECK(rmt_transmit(tx_ch, NULL, symbols, idx * sizeof(rmt_symbol_word_t), &tx_cfg));
    printf("NEC Power ON/OFF code sent");
}
