#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

SemaphoreHandle_t xMutex; //mutex handle    
int shared_counter = 0; //variable global shared betwen core   

void incrementTask(void *pvParameters) {
    while (1) {
        if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
            shared_counter++;
            printf("Increment Task | Core %d | Counter: %d\n", xPortGetCoreID(), shared_counter);
            xSemaphoreGive(xMutex);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void DecrementTask(void *pvParameters) {
    while (1) {
        if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
            shared_counter--;
            printf("Decrement Task | Core %d | Counter: %d\n", xPortGetCoreID(), shared_counter);
            xSemaphoreGive(xMutex);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void app_main(void)
{
    xMutex = xSemaphoreCreateMutex();
    if (xMutex==NULL) {
        printf("Mutex creation failed!\n");
        return;
    }

    xTaskCreatePinnedToCore(incrementTask, "incrementTask", 2048, NULL, 1, NULL, 0);

    //task b to core 1  
    xTaskCreatePinnedToCore(DecrementTask, "DecrementTask", 2048, NULL, 1, NULL, 1);
}


