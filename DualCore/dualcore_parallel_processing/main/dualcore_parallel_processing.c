#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/idf_additions.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_system.h"

#define LED 2   


void blinkTask(void *pvParameters) {
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    while (1) {
        gpio_set_level(LED, 1);
        printf("LED ON  | Core %d\n", xPortGetCoreID());
        vTaskDelay(pdMS_TO_TICKS(500));

        gpio_set_level(LED, 0);
        printf("LED OFF | Core %d\n", xPortGetCoreID());
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void counterTask(void *pvParameters) {
    int counter = 0;
    while (1) {
        printf("Counter: %d | Core %d\n", counter++, xPortGetCoreID());
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void)
{
    //task blink
    xTaskCreatePinnedToCore(blinkTask, "blink task", 2048, NULL, 1, NULL, 0);

    //task counter
    xTaskCreatePinnedToCore(counterTask, "tcounter task", 2048, NULL, 1, NULL, 1);
}

