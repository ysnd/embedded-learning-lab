#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED GPIO_NUM_2

void blink_task(void *pvParameter)
{
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    while (1) {
        gpio_set_level(LED, 1);
        vTaskDelay(pdMS_TO_TICKS(250));
        gpio_set_level(LED, 0);
        vTaskDelay(pdMS_TO_TICKS(250));
    }
}

void log_task(void *pvParameter) 
{
    while (1) {
        printf("Log: Hello from log_task!:\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


void app_main(void)
{
    xTaskCreate(blink_task, "blink_task", 2048, NULL, 2, NULL);
    xTaskCreate(log_task, "log_task", 2048, NULL, 1, NULL);
}
