#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/rmt.h"

#define RMT_TX_CH RMT_CHANNEL_0
#define RMT_TX_PIN 18
#define CARRIER_FREQ_HZ 38000 //38khz
#define DUTY_CYCLE 50
#define NEC_BITS 32

rmt_item32_t ir_burst(uint32_t duration_us) {
    uint32_t tick_high = duration_us * DUTY_CYCLE / 100 / 1;//1us per tick
    uint32_t tick_low = duration_us - tick_high;
    rmt_item32_t item = {
        .level0=1, .duration0 = tick_high,
        .level1=0, .duration1 = tick_low
    };
    return item;
}
//kirim 1 bit NEC
rmt_item32_t ir_bit(uint8_t bit) {
    if (bit) 
        return ir_burst(1690); // high 560 + low 1690
    else
        return ir_burst(560); // high 560 + low 560
}

void ir_encode(uint32_t code, rmt_item32_t *items) {
    for (int i=0; i<NEC_BITS; i++) {
        items[i] = ir_bit((code >> (31-i)) & 0x1);
    }
}

typedef struct {
    const char* name;
    uint32_t code;
} ir_button_t;

//daftar tombol ir  
ir_button_t buttons[] = {
    {"Power", 0x20DF10EF},
    {"VolUp", 0x20DF40BF},
    {"VolDown", 0x20DFC03F},
    {"Mute", 0x20DF906F}
};

    

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

    rmt_item32_t items[NEC_BITS];

    while (1) {
        for (int i = 0; i<sizeof(buttons)/sizeof(buttons[0]); i++) {
            printf("Sending IR Code: %s (0x%lX)\n", buttons[i].name, buttons[i].code);
            ir_encode(buttons[i].code, items);
            ESP_ERROR_CHECK(rmt_write_items(RMT_TX_CH, items, NEC_BITS, true));
            vTaskDelay(pdMS_TO_TICKS(3000));
        }
    }
}   

