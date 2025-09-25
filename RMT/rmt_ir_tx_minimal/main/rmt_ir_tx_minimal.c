#include <stdint.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/rmt.h"

#define RMT_TX_CH RMT_CHANNEL_0
#define RMT_TX_PIN 18
#define CARRIER_FREQ_HZ 38000 //38khz
#define DUTY_CYCLE 50

rmt_item32_t ir_burst(uint32_t duration_us) {
    uint32_t tick_high = duration_us * DUTY_CYCLE / 100 / 1;//1us per tick
    uint32_t tick_low = duration_us - tick_high;
    rmt_item32_t item = {
        .level0=1, .duration0 = tick_high,
        .level1=0, .duration1 = tick_low
    };
    return item;
}

rmt_item32_t ir_bit(uint8_t bit) {
    if (bit) 
        return ir_burst(1690); // high 560 + low 1690
    else
        return ir_burst(560); // high 560 + low 560
}

void app_main(void)
{
    printf("Initialize RMT TX on GPIO%d\n", RMT_TX_PIN);
    rmt_config_t cfg = {
        .rmt_mode = RMT_MODE_TX,
        .channel = RMT_TX_CH,
        .gpio_num = RMT_TX_PIN,
        .clk_div = 80, // 1 tick = 1us   
        .mem_block_num = 1,
        .tx_config.loop_en = false
    };
    rmt_config(&cfg);
    rmt_driver_install(cfg.channel, 0, 0);

    // contoh kirim code : nec 0x20DF10EF (tv power)
    uint32_t ir_code = 0x20DF10EF;
    rmt_item32_t items[32];
    for (int i=0; i<32; i++) {
        items[i] = ir_bit((ir_code >> (31-i)) & 0x1);
    }

    while (1) {
        printf("Sending IR Code 0x%lX\n", ir_code);
        ESP_ERROR_CHECK(rmt_write_items(RMT_TX_CH, items, 32, true));
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}   
