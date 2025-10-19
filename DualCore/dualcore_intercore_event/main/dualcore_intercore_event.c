#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#define EVENT_BIT_READY (1 << 0)
//declaration event group   
EventGroupHandle_t xEventGroup; 
//task core0: event 
void producerTask(void *pvParameters) {
    while (1) {
        printf("[Core0] producting event...\n");
        //set bit menjadi aktif
        xEventGroupSetBits(xEventGroup, EVENT_BIT_READY);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
//Task in core1 wait event  
void consumerTask(void *pvParameters) {
    while (1) {
        //wait until bit ready by core 0    
        EventBits_t bits = xEventGroupWaitBits(xEventGroup, EVENT_BIT_READY, pdTRUE, pdFALSE, portMAX_DELAY);

        if (bits & EVENT_BIT_READY) {
            printf("    [Core1] Event detected! doing work...\n");
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
}

void app_main(void)
{
    xEventGroup = xEventGroupCreate();
    if (xEventGroup==NULL) {
        printf("event_groups creation failed!\n");
        return;
    }

    xTaskCreatePinnedToCore(producerTask, "pTask", 2048, NULL, 1, NULL, 0);

    //task b to core 1  
    xTaskCreatePinnedToCore(consumerTask, "cTask", 2048, NULL, 1, NULL, 1);
}




