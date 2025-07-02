#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void low_priority_task(void *pvParameter)
{
    while (1) {
        printf("Task prioritas rendah berjalan...\n");
        taskYIELD();
    }
}

void high_priority_task(void *pvParameter)
{
    while (1) {
        printf(">>Task Prioritas tinggi berjalan ...\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

}

void app_main(void)
{
    xTaskCreate(low_priority_task, "low_task", 2048, NULL, 1, NULL);
    xTaskCreate(high_priority_task, "high_task", 2048, NULL, 2, NULL);
}
