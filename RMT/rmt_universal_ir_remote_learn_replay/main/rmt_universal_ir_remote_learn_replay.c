#include <stdio.h>
#include <string.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/rmt.h"
#include "esp_log.h"

#define RMT_RX_CHANNEL RMT_CHANNEL_0
#define RMT_RX_GPIO 19
#define RMT_TX_CHANNEL RMT_CHANNEL_1
#define RMT_TX_GPIO 18
#define RMT_CLK_DIV 80   

static rmt_item32_t ir_buffer[256];
static size_t ir_buffer_len = 0;

static void print_items(const char *label, rmt_item32_t *items, size_t len) {
    printf("%s (len=%d):",label, (int)len);
    for (size_t i =0; i < len; i++) {
        printf("[%02d] level0=%d, duration0=%5d us | level1=%d, duration1=%5d us\n",(int)i, items[i].level0, items[i].duration0, items[i].level1, items[i].duration1);
    }
}

static void rx_task(void *arg) {
    rmt_config_t rmt_rx;
    rmt_rx.channel = RMT_RX_CHANNEL;
    rmt_rx.gpio_num = RMT_RX_GPIO;
    rmt_rx.clk_div = RMT_CLK_DIV;
    rmt_rx.mem_block_num = 1;
    rmt_rx.rmt_mode = RMT_MODE_RX;
    rmt_rx.rx_config.filter_en = true;
    rmt_rx.rx_config.filter_ticks_thresh = 100;
    rmt_rx.rx_config.idle_threshold = 10000;

    ESP_ERROR_CHECK(rmt_config(&rmt_rx));
    ESP_ERROR_CHECK(rmt_driver_install(RMT_RX_CHANNEL, 1000, 0));
    

    RingbufHandle_t rb = NULL;
    ESP_ERROR_CHECK(rmt_get_ringbuf_handle(RMT_RX_CHANNEL, &rb));
    rmt_rx_start(RMT_RX_CHANNEL, true);

    while (1) {
        size_t rx_size = 0;
        rmt_item32_t *items = (rmt_item32_t *)xRingbufferReceive(rb, &rx_size, 1000 / portTICK_PERIOD_MS);
        if (items) {
            ir_buffer_len = rx_size / sizeof(rmt_item32_t);
            if (ir_buffer_len > 256) ir_buffer_len = 256;
            memcpy(ir_buffer, items, ir_buffer_len * sizeof(rmt_item32_t));
            vRingbufferReturnItem(rb, (void *)items);
            printf("Captured IR frame, len=%d items\n", (int)ir_buffer_len);
            print_items("Captured\n", ir_buffer, ir_buffer_len);
        }
    }
}

static void tx_send_ir(void) {
    if (ir_buffer_len ==0) {
        printf("Belum ada data IR yang dipelajari\n");
        return;
    }

    rmt_config_t rmt_tx;
    rmt_tx.channel = RMT_TX_CHANNEL;
    rmt_tx.gpio_num = RMT_TX_GPIO;
    rmt_tx.clk_div = RMT_CLK_DIV;
    rmt_tx.mem_block_num = 1;
    rmt_tx.tx_config.loop_en = false;
    rmt_tx.tx_config.carrier_en = true;
    rmt_tx.tx_config.carrier_freq_hz = 38000;
    rmt_tx.tx_config.carrier_duty_percent = 33;
    rmt_tx.tx_config.carrier_level = RMT_IDLE_LEVEL_HIGH;
    rmt_tx.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;
    rmt_tx.tx_config.idle_output_en = true;

    ESP_ERROR_CHECK(rmt_config(&rmt_tx));
    ESP_ERROR_CHECK(rmt_driver_install(RMT_TX_CHANNEL, 0, 0));

    printf("Replaying IR Frame...\n");
    print_items("Transmitting\n", ir_buffer, ir_buffer_len);

    ESP_ERROR_CHECK(rmt_write_items(RMT_TX_CHANNEL, ir_buffer, ir_buffer_len, true));
    ESP_ERROR_CHECK(rmt_wait_tx_done(RMT_TX_CHANNEL, portMAX_DELAY));
    printf("Done replay\n");
}

void app_main(void) {
    xTaskCreate(rx_task, "rx_task", 4096, NULL, 10, NULL);

    printf("Arahkan remote ke GPIO %d, tekan tombol...\n", RMT_RX_GPIO);
    tx_send_ir();
}
