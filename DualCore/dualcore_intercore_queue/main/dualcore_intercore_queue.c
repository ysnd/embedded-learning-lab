#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

static QueueHandle_t xQueue = NULL; //queue handle      

void senderTask(void *pvParameters) {
    int counter = 0;
    while (1) {
        counter++;
        if (xQueueSend(xQueue, &counter, portMAX_DELAY)==pdPASS){
            printf("[Core0] Sent: %d\n", counter);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void receiverTask(void *pvParameters) {
    int receivedVal;
    while (1) {
        if (xQueueReceive(xQueue, &receivedVal, portMAX_DELAY)==pdPASS) {
            printf("    [Core1] Received: %d\n", receivedVal);
        }
    }
}

void app_main(void)
{
    xQueue = xQueueCreate(10, sizeof(int));
    if (xQueue==NULL) {
        printf("Queue creation failed!\n");
        return;
    }

    xTaskCreatePinnedToCore(senderTask, "sTask", 2048, NULL, 1, NULL, 0);

    //task b to core 1  
    xTaskCreatePinnedToCore(receiverTask, "rTask", 2048, NULL, 1, NULL, 1);
}



