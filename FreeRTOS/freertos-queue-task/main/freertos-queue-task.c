#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"


#define SENSDELAY 1000

QueueHandle_t data_queue;

void sensor_task(void *pvParameters) {
    int val = 0;

    while (1) {
        val++;
        printf("Sensor Task: value = %d\n", val);
        xQueueSend(data_queue, &val, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(SENSDELAY));
    }
}

void print_task(void *pvParameters) {
    int received;

    while (1) {
        if (xQueueReceive(data_queue, &received, portMAX_DELAY)) {
            printf("Print Task: Received = %d\n", received);
        
        }
    }
}

void app_main(void)
{
    //buat queue yang bisa simpen 10 integer
    data_queue = xQueueCreate(10, sizeof(int));

    if (data_queue == NULL) {
        printf("Queue creation failed!\n");
        return;
    }
    xTaskCreate(sensor_task, "Sensor_Task", 2048, NULL, 1, NULL);
    xTaskCreate(print_task, "print_task", 2048, NULL, 1, NULL);

}
