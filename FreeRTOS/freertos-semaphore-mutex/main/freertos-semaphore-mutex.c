#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

SemaphoreHandle_t uart_mutex;

void task1(void *pvParameter) {
    while (1) {
        if (xSemaphoreTake(uart_mutex, pdMS_TO_TICKS(100))) {
            printf("Task 1 mengakses UART\n");
            vTaskDelay(pdMS_TO_TICKS(500));
            xSemaphoreGive(uart_mutex);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void task2(void *pvParameter) {
    while (1) {
        if (xSemaphoreTake(uart_mutex, pdMS_TO_TICKS(100))) {
            printf("Task 2 mengakses UART\n");
            vTaskDelay(pdMS_TO_TICKS(500));
            xSemaphoreGive(uart_mutex);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void app_main(void)
{
    uart_mutex = xSemaphoreCreateMutex();

    xTaskCreate(task1, "Task 1", 2048, NULL, 2, NULL);
    xTaskCreate(task2, "Task 2", 2048, NULL, 2, NULL);
}
