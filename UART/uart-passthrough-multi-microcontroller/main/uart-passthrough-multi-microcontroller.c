#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"


#define UARTP UART_NUM_0
#define UARTT UART_NUM_1
#define BUF_SIZE 1024
#define TXDP GPIO_NUM_17
#define RXDP GPIO_NUM_16

void app_main(void)
{
    uart_config_t conf = {
        .baud_rate  = 115200,
        .data_bits  = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits  = UART_STOP_BITS_1,
        .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE
    };

    uart_param_config(UARTT, &conf);
    uart_set_pin(UARTT, TXDP, RXDP, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UARTT, BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_driver_install(UARTP, BUF_SIZE * 2, 0, 0, NULL, 0);
    uint8_t data_from_pc[BUF_SIZE];
    uint8_t data_from_target[BUF_SIZE];

    while (1) {
        int len_pc = uart_read_bytes(UARTP, data_from_pc, BUF_SIZE, 10 / portTICK_PERIOD_MS);
        if (len_pc >0) {
            uart_write_bytes(UARTT, (const char *)data_from_pc, len_pc);
        }
        int len_target = uart_read_bytes(UARTT, data_from_target, BUF_SIZE, 10 / portTICK_PERIOD_MS);
        if (len_target > 0) {
            uart_write_bytes(UARTP, (const char *)data_from_target, len_target);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
