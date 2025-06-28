#include <stdio.h>
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/uart.h"

#define UARTP UART_NUM_0
#define BUF_SIZE 128
#define LED GPIO_NUM_2

void init_uart() {
    uart_config_t conf = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UARTP, &conf);
    uart_driver_install(UARTP, BUF_SIZE * 2, 0, 0, NULL, 0);
}

void init_led() {
    gpio_reset_pin(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    gpio_set_level(LED, 0);
}

void app_main(void)
{
    init_uart();
    init_led();

    uint8_t data[BUF_SIZE];
    bool led_state = false;

    while (1) {
        int len = uart_read_bytes(UARTP, data, BUF_SIZE - 1, 100 / portTICK_PERIOD_MS);
        if (len > 0) {
            data[len] = '\0';
            printf("Received : %s", data);

            if (strstr((char*)data, "LED ON")) {
                gpio_set_level(LED, 1);
                led_state = true;
                uart_write_bytes(UARTP, "LED is ON\n", strlen("LED is ON\n"));
            }
            else if (strstr((char*)data, "LED OFF")) {
                gpio_set_level(LED, 0);
                led_state = false;
                uart_write_bytes(UARTP, "LED is OFF\n", strlen("LED is OFF\n"));
            }
            else if (strstr((char*)data, "STATUS")) {
                if (led_state) {
                    uart_write_bytes(UARTP, "LED is ON\n",strlen("LED is ON\n"));
                }
                else {
                    uart_write_bytes(UARTP, "LED is OFF\n", strlen("LED is OFF"));
                }
            }
            else {
                uart_write_bytes(UARTP, "Unknown command\n", strlen("Unknown command\n"));
            }
        }   
    }
}
