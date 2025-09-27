#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/rmt.h"
#include "esp_log.h"

#define RMT_RX_CH RMT_CHANNEL_0
#define RMT_RX_PIN 19
#define RMT_CLK_DIV 80
#define RMT_RX_BUFFER_SIZE 2048

static bool nec_check_in_range(int duration_ticks, int target_us, int tolerance_us) {
    int duration_us = duration_ticks *1; //karena clk_div_80 ->1tick = 1us  
    return (duration_us > (target_us - tolerance_us)) && (duration_us < (target_us + tolerance_us));
}

static int nec_decode(rmt_item32_t *items, int num_items) {
    if (num_items <2) {
        printf("Not Enough RMT Items (%d)\n", num_items);
        return -1;
    }
    if (nec_check_in_range(items[0].duration0, 9000, 200) && nec_check_in_range(items[0].duration1, 2250, 200)) {
        printf("NEC Repeat code detected!\n");
        return -2;
    }

    if (num_items < 34) { //1 leader + 32 bits+stop
       printf("Not Enough RMT items (%d)\n", num_items);
       return -1;
    }

    //cek leader pulse
    if (!nec_check_in_range(items[0].duration0, 9000, 600) || !nec_check_in_range(items[0].duration1, 4500, 600)) {
        printf("Invalid leader pulse H:%dus L:%dus\n", items[0].duration0, items[0].duration1);
        return -1;
    }

    uint32_t data = 0;
    for (int i = 1; i<= 32; i++) {
        if (!nec_check_in_range(items[i].duration0, 560, 250)) {
            printf("Invalid mark duratuion at bit %d\n", i);
            return -1;
        }

        if (nec_check_in_range(items[i].duration1, 560, 250)) {
            //bit0
            data = (data << 1) | 0;
        } else if (nec_check_in_range(items[i].duration1, 1690, 500)) {
            //bit1
            data = (data << 1) | 1;
        } else {
            printf("Invalid space duration at bit %d: %d\n", i, items[i].duration1);
        }
    }

    uint8_t addr = (data >> 24) & 0xFF;
    uint8_t addr_inv = (data >> 16) & 0xFF;
    uint8_t cmd = (data >> 8) & 0xFF;
    uint8_t cmd_inv = (data >> 0) & 0xFF;

    if ((addr ^ addr_inv) != 0xFF || (cmd ^ cmd_inv) != 0xFF) {
        printf("Adress/Command checksum failed! addr=0x%02X cmd=0x%02X\n", addr, cmd);
        return -1;
    }
    printf("NEC decoded -> Adress=0x%02X, Command=0x%02X\n", addr, cmd);
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
    rmt_rx.rx_config.idle_threshold = 20000;

    ESP_ERROR_CHECK(rmt_config(&rmt_rx));
    ESP_ERROR_CHECK(rmt_driver_install(rmt_rx.channel, RMT_RX_BUFFER_SIZE, 0));

    RingbufHandle_t rb = NULL;
    ESP_ERROR_CHECK(rmt_get_ringbuf_handle(rmt_rx.channel, &rb));
    rmt_rx_start(rmt_rx.channel, true);

    printf("RMT RX Started, ready to receive NEC remote\n");
    int last_cmd = -1;

    while (1) {
        size_t rx_size = 0;
        rmt_item32_t* items = (rmt_item32_t*) xRingbufferReceive(rb, &rx_size, pdMS_TO_TICKS(2000));
        if (items) {
            int num_items = rx_size / sizeof(rmt_item32_t);
            int result = nec_decode(items, num_items);
            if (result >=0) {
                last_cmd=result;
            } else if (result == -2 && last_cmd >=0) {
                printf("NEC Repeat for last cmd=0x%02X\n", last_cmd);
            }
            vRingbufferReturnItem(rb, (void*)items);
        }
    }
}

