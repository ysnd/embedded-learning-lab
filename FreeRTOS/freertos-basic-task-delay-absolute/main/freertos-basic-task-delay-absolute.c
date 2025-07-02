#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "portmacro.h"


#define LED GPIO_NUM_2

void precise_blink_task(void *pvParameter)
{
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(500);

    while (1) {
        gpio_set_level(LED, 1);
        vTaskDelayUntil(&xLastWakeTime, xFrequency / 2);

        gpio_set_level(LED, 0);
        vTaskDelayUntil(&xLastWakeTime, xFrequency /2);
    }
}

void task2(void *pvParameter)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    TickType_t xFrequency = pdMS_TO_TICKS(500);

    while (1) {
        printf("Hallo ini task2 2 detik\n");
        vTaskDelayUntil(&xLastWakeTime, xFrequency * 2);
    }
}

void app_main(void)
{
    xTaskCreate(precise_blink_task, "precise_blink", 2048, NULL, 2, NULL);
    xTaskCreate(task2, "task2", 2048, NULL, 2, NULL);
}
