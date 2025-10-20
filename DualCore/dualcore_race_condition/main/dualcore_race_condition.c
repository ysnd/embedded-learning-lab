#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

volatile int shared_counter = 0; //variable global shared betwen core   

void incrementTask(void *pvParameters) {
    while (1) {
        shared_counter++;
        printf("Increment Task | Core %d | Counter: %d\n", xPortGetCoreID(), shared_counter);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void DecrementTask(void *pvParameters) {
    while (1) {
        shared_counter--;
        printf("Decrement Task | Core %d | Counter: %d\n", xPortGetCoreID(), shared_counter);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void app_main(void)
{
    xTaskCreatePinnedToCore(incrementTask, "incrementTask", 2048, NULL, 1, NULL, 0);

    //task b to core 1  
    xTaskCreatePinnedToCore(DecrementTask, "DecrementTask", 2048, NULL, 1, NULL, 1);
}

