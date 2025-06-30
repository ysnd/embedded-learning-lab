#include <stdio.h>
#include <strings.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "driver/gpio.h"


#define UARTN UART_NUM_1
#define TXDP GPIO_NUM_17
#define RXDP GPIO_NUM_16
#define BUF_SIZE 1024

static const char *TAG = "UART_EVENT";

static QueueHandle_t uart_queue;

static void uart_event_task(void *pvParameters) {
    uart_event_t event;
    uint8_t data[BUF_SIZE];
    while (1) {
        if (xQueueReceive(uart_queue, (void *)&event, portMAX_DELAY)) {
            bzero(data, BUF_SIZE);
            switch (event.type) {
                case UART_DATA:
                    ESP_LOGI(TAG, "Received %d bytes", event.size);
                    uart_read_bytes(UARTN, data, event.size, portMAX_DELAY);
                    ESP_LOGI(TAG, "Data: %.*s", event.size, data);
                    break;

                case UART_FIFO_OVF:
                    ESP_LOGW(TAG, "HW FIFO Overflow");
                    uart_flush_input(UARTN);
                    xQueueReset(uart_queue);
                    break;

                case UART_BUFFER_FULL:
                    ESP_LOGW(TAG, "Ring buffer full");
                    uart_flush_input(UARTN);
                    xQueueReset(uart_queue);
                    break;

                case UART_BREAK:
                    ESP_LOGW(TAG, "UART Break");
                    break;

                case UART_PARITY_ERR:
                    ESP_LOGE(TAG, "Parity Error");
                    break;

                case UART_FRAME_ERR:
                    ESP_LOGE(TAG, "Freme error");
                    break;

                default:
                    ESP_LOGI(TAG, "Other event: %d", event.type);
                    break;
            }
        }
    }
}

void app_main(void)
{
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
    
    ESP_LOGI(TAG, "UART EVENT TASK STARTED");
}
