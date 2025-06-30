#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "driver/gpio.h"


#define UARTN UART_NUM_0
#define TXDP UART_PIN_NO_CHANGE
#define RXDP UART_PIN_NO_CHANGE
#define BUF_SIZE 1024
#define LED GPIO_NUM_2

static const char *TAG = "UART_CMD";

static QueueHandle_t uart_queue;

static void handle_cmd(const char *cmd) {
    if (strncmp(cmd, "led on", 6) == 0) {
        gpio_set_level(LED, 1);
        ESP_LOGI(TAG, "LED Turned ON\n");
    }
    else if (strncmp(cmd, "led off", 7) == 0) {
        gpio_set_level(LED, 0);
        ESP_LOGI(TAG, "LED Turned OFF\n");
    }
    else {
        ESP_LOGW(TAG, "Unknown command: %s\n", cmd);
    }
}

static void uart_event_task(void *pvParameters) {
    uart_event_t event;
    uint8_t data[BUF_SIZE];
    while (1) {
        if (xQueueReceive(uart_queue, &event, portMAX_DELAY)) {
            bzero(data, BUF_SIZE);
            switch (event.type) {
                case UART_DATA: {
                    int len = uart_read_bytes(UARTN, data, event.size, portMAX_DELAY);
                    data[len] = '\0';
                    ESP_LOGI(TAG, "Received: %s\n", data);
                    handle_cmd((char *)data);
                    break;
                }

                case UART_FIFO_OVF:
                case UART_BUFFER_FULL:
                    ESP_LOGW(TAG, "Ring buffer full\n");
                    uart_flush_input(UARTN);
                    xQueueReset(uart_queue);
                    break;
                default:
                    break;
            }
        }
    }
}

void app_main(void) 
{
    gpio_config_t gcon = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << LED),
    };
    gpio_config(&gcon);

    uart_config_t conf = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    uart_param_config(UARTN, &conf);
    uart_set_pin(UARTN, TXDP, RXDP, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UARTN, BUF_SIZE * 2, BUF_SIZE * 2, 20, &uart_queue, 0);

    xTaskCreate(uart_event_task, "uart_event_task", 4096, NULL, 10, NULL);
    
    ESP_LOGI(TAG, "UART Command Parser started..");
}
