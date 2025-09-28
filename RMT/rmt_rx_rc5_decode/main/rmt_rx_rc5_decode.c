#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/rmt.h"
#include "esp_log.h"

#define RMT_RX_CH RMT_CHANNEL_0
#define RMT_RX_PIN 19
#define RMT_CLK_DIV 80
#define RMT_RX_BUFFER_SIZE 2048

// RC5: 1 bit = 1778 us, half-bit = 889us
#define RC5_HALF_BIT_US 889
#define RC5_TOLERANCE_US 300

static bool rc5_check_in_range(int duration_ticks, int target_us, int tolerance_us) {
    int duration_us = duration_ticks *1; //karena clk_div_80 ->1tick = 1us  
    return (duration_us > (target_us - tolerance_us)) && (duration_us < (target_us + tolerance_us));
}

static int rc5_decode(rmt_item32_t *items, int num_items) {

    uint16_t data = 0;
    int bit_count = 0;
    //decode manchester: tiap bit =2 level(half bit)
    for (int i = 1; i<= num_items; i +=2) {
        if (!rc5_check_in_range(items[i].duration0 + items[i].duration1, RC5_HALF_BIT_US * 2, RC5_TOLERANCE_US)) {
            printf("Invalid bit timing at item %d\n", i);
            return -1;
        }
        //manchester decoding
        if (items[i].level0 == 1 && items[i].level1 == 0) {
          //high-. low = 1            
          data = (data << 1) | 1; 
        } else if (items[i].level0==0 && items[i].level1==1) {
            //low->high = 0
            data = (data << 1) | 0;
        } else {
            printf("Invalid manchester sequence at %d\n", i);
            return -1;
        }
        bit_count++;
        if (bit_count >=14) break;
    }

    if (bit_count < 14) {
        printf("Too few bits decoded (%d)\n", bit_count);
        return -1;
    }
    
    int start1 = (data >> 13) & 1;
    int start2 = (data >> 12) & 1;
    int toggle = (data >> 11) & 1;
    uint8_t cmd = (data >> 0) & 0x3F;
    uint8_t addr = (data >> 6) & 0x1F;

    printf("RC5 decoded -> Start=%d%d, Toglle=%d, Addr=0x%02X, Cmd:0x%02X\n", start1, start2, toggle, addr, cmd);
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
    rmt_rx.rx_config.idle_threshold = 5000;

    ESP_ERROR_CHECK(rmt_config(&rmt_rx));
    ESP_ERROR_CHECK(rmt_driver_install(rmt_rx.channel, RMT_RX_BUFFER_SIZE, 0));

    RingbufHandle_t rb = NULL;
    ESP_ERROR_CHECK(rmt_get_ringbuf_handle(rmt_rx.channel, &rb));
    rmt_rx_start(rmt_rx.channel, true);

    printf("RMT RX Started, waiting for RC5 remote\n");

    while (1) {
        size_t rx_size = 0;
        rmt_item32_t* items = (rmt_item32_t*) xRingbufferReceive(rb, &rx_size, pdMS_TO_TICKS(2000));
        if (items) {
            int num_items = rx_size / sizeof(rmt_item32_t);
            rc5_decode(items, num_items);
            vRingbufferReturnItem(rb, (void*)items);
        }
    }
}



