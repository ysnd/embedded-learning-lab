#include <stdio.h>
#include "driver/uart.h"
#include "driver/gpio.h"
#include "string.h"

#define LED GPIO_NUM_2
#define BUF_SIZE 1024

void app_main(void)
{
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);

    const uart_config_t uart_conf = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_0, &uart_conf);
    uart_driver_install(UART_NUM_0, BUF_SIZE * 2, 0, 0, NULL, 0);

    uint8_t data[BUF_SIZE];
    while (1) {
        int len = uart_read_bytes(UART_NUM_0, data, BUF_SIZE - 1, pdMS_TO_TICKS(500));
        if (len>0) {
        data[len] = '\0';
        if (strstr((char*)data, "LED ON")) {
            gpio_set_level(LED, 1);
            uart_write_bytes(UART_NUM_0, "ACK LED ON\n", strlen("ACK LED ON\n"));
            }
        else if (strstr((char*)data, "LED OFF")) {
            gpio_set_level(LED, 0);
            uart_write_bytes(UART_NUM_0, "ACK LED OFF\n", strlen("ACK LED OFF\n"));
            }
        else {
            uart_write_bytes(UART_NUM_0, "unkonwn cmd\n", strlen("unkonwn cmd\n"));
            }  
        }
    }
}
