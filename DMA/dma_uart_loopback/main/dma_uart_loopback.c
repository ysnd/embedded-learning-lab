#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"

#define UART_PORT UART_NUM_1
#define UART_TX_PIN 17
#define UART_RX_PIN 16
#define DMA_BUF_SIZE 1024

void app_main(void)
{
    //cnfigure uart
    uart_config_t uart_cfg = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };

    //install UART driver with dma buffer
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT, DMA_BUF_SIZE * 2, 0, 0, NULL, ESP_INTR_FLAG_IRAM));
    ESP_ERROR_CHECK(uart_param_config(UART_PORT, &uart_cfg));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    printf("UART DMA Loopback ready \n");

    uint8_t tx_data[] = "Hello DMA UART!\n";
    uint8_t rx_buffer[DMA_BUF_SIZE];
    while (1) {
        // send data via tx 
        uart_write_bytes(UART_PORT, (const char*)tx_data, sizeof(tx_data));
        //read from rx  
        int len = uart_read_bytes(UART_PORT, rx_buffer, sizeof(rx_buffer), pdMS_TO_TICKS(500));
        if (len > 0) {
            printf("RX (%d bytes): ", len);
            for (int i = 0; i < len; i++) {
                printf("%c", rx_buffer[i]);
            }
            printf("\n");
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
