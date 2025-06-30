#include <stdint.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_intr_alloc.h"
#include "hal/uart_types.h"
#include "soc/uart_struct.h"

#define BUF_SIZE 1024

static const char *TAG = "uart_isr";

static void IRAM_ATTR uart_intr_handle(void* arg) {
    uint16_t rx_fifo_len;
    uint32_t status = UART1.int_st.val;
    uint8_t data[128];

    rx_fifo_len = UART1.status.rxfifo_cnt;
    for (int i = 0; i < rx_fifo_len && i < sizeof(data - 1); i++) {
        data[i] = UART1.fifo.rw_byte;
    }
    data[rx_fifo_len] = '\0';
    UART1.int_clr.val = status;
}

void app_main(void)
{
    const uart_config_t conf = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    
    uart_param_config(UART_NUM_1, &conf);
    uart_set_pin(UART_NUM_1, 17, 16, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM_1, BUF_SIZE, 0, 0, NULL, 0);

    ESP_ERROR_CHECK(uart_isr_register(UART_NUM_1, uart_intr_handle, NULL, ESP_INTR_FLAG_IRAM, NULL));
    uart_enable_rx_intr(UART_NUM_1);
    ESP_LOGI(TAG, "UART1 interruupt setup complete");
}
