#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

void taskA(void *pvParameters) {
    while (1) {
        printf("Task A running on core %d\n", xPortGetCoreID());
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void taskB(void *pvParameters) {
    while (1) {
        printf("Task B running on core %d\n", xPortGetCoreID());
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void)
{
    xTaskCreate(taskA, "taska", 2048, NULL, 1, NULL);

    //task b to core 1  
    xTaskCreatePinnedToCore(taskB, "taskB", 2048, NULL, 1, NULL, 1);
}
