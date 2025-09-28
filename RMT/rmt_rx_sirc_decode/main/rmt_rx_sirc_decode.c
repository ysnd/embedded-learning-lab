#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/rmt.h"
#include "esp_log.h"

#define RMT_RX_CH RMT_CHANNEL_0
#define RMT_RX_PIN 19
#define RMT_CLK_DIV 80
#define RMT_RX_BUFFER_SIZE 2048

static bool sirc_check_in_range(int duration_ticks, int target_us, int tolerance_us) {
    int duration_us = duration_ticks *1; //karena clk_div_80 ->1tick = 1us  
    return (duration_us > (target_us - tolerance_us)) && (duration_us < (target_us + tolerance_us));
}

static int sirc_decode(rmt_item32_t *items, int num_items) {

    if (num_items < 12) { //minimal frame (leader +12 bit)
       printf("Not Enough RMT items (%d)\n", num_items);
       return -1;
    }

    //cek leader pulse:on sekitar 2400
    if (!sirc_check_in_range(items[0].duration0, 2400, 400)) {
        printf("Invalid leader pulse H:%dus\n", items[0].duration0);
        return -1;
    }

    uint32_t data = 0;
    int bit_count = 0;
    //decode bit lsb first
    for (int i = 1; i<= num_items; i++) {
        if (sirc_check_in_range(items[i].duration0, 600, 300)) {
            if (sirc_check_in_range(items[i].duration1, 600, 300)) {
                //bit0 = 600us on +600us off
                data |= (0 << bit_count); 
            } else if (sirc_check_in_range(items[i].duration1, 1200, 300)) {
                //bit1
                data |= (1 << bit_count);
            } else {
                printf("Invalid space duration at bit %d: %d\n", bit_count, items[i].duration1);
                return -1;
            }
            bit_count++;
        } else {
            break; //pulse sudah tidak valid
        }
    }

    if (bit_count < 12) {
        printf("Too few bits decoded (%d)\n", bit_count);
        return -1;
    }

    uint8_t cmd = data & 0x7F;
    uint8_t addr = (data >> 7) & 0x1F;

    printf("SIRC decoded -> Adress=0x%02X, Command=0x%02X, Bits=%d\n", addr, cmd, bit_count);
    return cmd;
}

void app_main(void) {
    rmt_config_t rmt_rx;
    rmt_rx.channel = RMT_RX_CH;
    rmt_rx.gpio_num = RMT_RX_PIN;
    rmt_rx.clk_div = RMT_CLK_DIV;
    rmt_rx.mem_block_num = 1;
    rmt_rx.rmt_mode = RMT_MODE_RX;

    rmt_rx.rx_config.filter_en = true;
    rmt_rx.rx_config.filter_ticks_thresh = 100;
    rmt_rx.rx_config.idle_threshold = 10000;

    ESP_ERROR_CHECK(rmt_config(&rmt_rx));
    ESP_ERROR_CHECK(rmt_driver_install(rmt_rx.channel, RMT_RX_BUFFER_SIZE, 0));

    RingbufHandle_t rb = NULL;
    ESP_ERROR_CHECK(rmt_get_ringbuf_handle(rmt_rx.channel, &rb));
    rmt_rx_start(rmt_rx.channel, true);

    printf("RMT RX Started, waiting for Sony SIRC remote\n");

    while (1) {
        size_t rx_size = 0;
        rmt_item32_t* items = (rmt_item32_t*) xRingbufferReceive(rb, &rx_size, pdMS_TO_TICKS(2000));
        if (items) {
            int num_items = rx_size / sizeof(rmt_item32_t);
            sirc_decode(items, num_items);
            vRingbufferReturnItem(rb, (void*)items);
        }
    }
}


