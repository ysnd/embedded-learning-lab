#include <stdio.h>
#include <string.h>
#include "driver/uart.h"
#include "driver/gpio.h"

#define TXDP GPIO_NUM_17
#define RXDP GPIO_NUM_16

void app_main(void)
{
    const int uart_num = UART_NUM_1;
    uart_config_t conf = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(uart_num, &conf);
    uart_set_pin(uart_num, TXDP, RXDP, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(uart_num, 1024, 0, 0, NULL, 0);

    const char *cmd1 = "LED ON\n";
    const char *cmd2 = "LED OFF\n";
    const char *cmd3 = "sada\n";

    while (1) {
        uart_write_bytes(uart_num, cmd1, strlen(cmd1));
        vTaskDelay(pdMS_TO_TICKS(1000));
        uart_write_bytes(uart_num, cmd2, strlen(cmd2));
        vTaskDelay(pdMS_TO_TICKS(1000));
        uart_write_bytes(uart_num, cmd3, strlen(cmd3));
    }
}
