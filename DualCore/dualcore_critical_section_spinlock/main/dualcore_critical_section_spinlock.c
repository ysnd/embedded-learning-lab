#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

//spinlock object
portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;
volatile int shared_counter = 0; //variable global shared betwen core   

void Taskcore0(void *pvParameters) {
    int local_counter;
    while (1) {
        portENTER_CRITICAL(&spinlock);
        shared_counter++;
        local_counter = shared_counter; 
        portEXIT_CRITICAL(&spinlock);
        printf("Increment Task Core 0 | Counter: %d\n", local_counter);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void Taskcore1(void *pvParameters) {
    int local_counter;
    while (1) {
        portENTER_CRITICAL(&spinlock);
        shared_counter--;
        local_counter = shared_counter; 
        portEXIT_CRITICAL(&spinlock);
        printf("Decrement Task Core 1 | Counter: %d\n", local_counter);
        vTaskDelay(pdMS_TO_TICKS(300));
    }
}

void app_main(void)
{
    xTaskCreatePinnedToCore(Taskcore0, "Task0", 2048, NULL, 2, NULL, 0);

    //task b to core 1  
    xTaskCreatePinnedToCore(Taskcore1, "Task1", 2048, NULL, 2, NULL, 1);
}


