#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"


SemaphoreHandle_t recursive_mutex;

void level2() {
    xSemaphoreTakeRecursive(recursive_mutex, portMAX_DELAY);
    printf(" > [Level 2] Inside nested critical section\n");
    vTaskDelay(pdMS_TO_TICKS(100));
    xSemaphoreGiveRecursive(recursive_mutex);
}

void level1_task(void *pvParameters) {
    while (1) {
        xSemaphoreTakeRecursive(recursive_mutex, portMAX_DELAY);
        printf("[Level 1] Enter critical section\n");
        level2();
        printf("[Level 1] Exit critical section\n");
        xSemaphoreGiveRecursive(recursive_mutex);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void)
{
    recursive_mutex = xSemaphoreCreateRecursiveMutex();
    if (recursive_mutex == NULL) {
        printf("Failed to create recursive mutex!\n");
        return;
    }
    xTaskCreate(level1_task, "level1 task", 2048, NULL, 1, NULL);
}
