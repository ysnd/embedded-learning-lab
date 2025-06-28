#include <stdio.h>
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/projdefs.h"


#define UARTP UART_NUM_1
#define TXDP (GPIO_NUM_17)
#define RXDP (GPIO_NUM_16)
#define BUF_SIZE (1024)

void app_main(void)
{
    const uart_config_t uart_conf = {
        .baud_rate  = 115200,
        .data_bits  = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits  = UART_STOP_BITS_1,
        .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,
    };
    uart_param_config(UARTP, &uart_conf);
    uart_set_pin(UARTP, TXDP, RXDP, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UARTP, BUF_SIZE * 2, 0, 0, NULL, 0);

    const char* test_str = "ESP UART Loopback\n";
    uart_write_bytes(UARTP, test_str, strlen(test_str));

    uint8_t data[BUF_SIZE];

    while (1) {
        int len = uart_read_bytes(UARTP, data, BUF_SIZE, 100 / portTICK_PERIOD_MS);
        if (len>0) {
            data[len] = '\0';
            ESP_LOGI("UART", "Received: %s", (char*) data);
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
