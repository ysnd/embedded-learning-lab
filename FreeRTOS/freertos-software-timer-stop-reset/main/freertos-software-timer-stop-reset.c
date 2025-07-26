#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

TimerHandle_t myTimer;

void timerCallback(TimerHandle_t xTimer) {
    printf("Timer expired!\n");
}

void controlTask(void *pvParameters) {
    vTaskDelay(pdMS_TO_TICKS(3000));
    printf("Resetting Timer...\n");
    xTimerReset(myTimer, 0);

    vTaskDelay(pdMS_TO_TICKS(2000));
    printf("Stopping Timer...\n");
    xTimerStop(myTimer, 0);

    vTaskDelete(NULL);
}

void app_main(void)
{
    myTimer = xTimerCreate("Timer", 
            pdMS_TO_TICKS(5000), 
            pdTRUE, 
            NULL, 
            timerCallback);
    if (myTimer != NULL) {
        xTimerStart(myTimer, 0);
    }
    xTaskCreate(controlTask, "ControlTask", 2048, NULL, 1, NULL);
}
