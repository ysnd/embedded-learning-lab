#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "string.h"

#define UARTP UART_NUM_0
#define UARTBR 115200
#define TXDP UART_PIN_NO_CHANGE
#define RXDP UART_PIN_NO_CHANGE


void app_main(void)
{
    uart_config_t uart_config = {
        .baud_rate  = UARTBR,
        .data_bits  = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits  = UART_STOP_BITS_1,
        .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UARTP, &uart_config);
    uart_set_pin(UARTP, TXDP, RXDP, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UARTP, 1024, 0, 0, NULL, 0);

    const char *message = "Hello from UART!\r\n";
    while (1) {
        uart_write_bytes(UARTP, message, strlen(message));
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
