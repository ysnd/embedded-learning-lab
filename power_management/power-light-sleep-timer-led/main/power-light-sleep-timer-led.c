#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_sleep.h"
#include "driver/gpio.h"

#define LED 2
#define BTN 0

void blink_task(void *pvParameters) {
    while (1) {
        gpio_set_level(LED, 1);
        vTaskDelay(pdMS_TO_TICKS(500));
        gpio_set_level(LED, 0);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void app_main(void)
{
    gpio_reset_pin(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);

    gpio_reset_pin(BTN);
    gpio_set_direction(BTN, GPIO_MODE_INPUT);
    gpio_pullup_en(BTN);

    xTaskCreate(blink_task, "Blink", 2048, NULL, 5, NULL);

    while (1) {
        printf("Going to light sleep (5s or buttn press)...\n");

        //enabke wakeup_soources
        esp_sleep_enable_timer_wakeup(10000000); // 5 s  
        esp_sleep_enable_ext0_wakeup(BTN, 0);
        //enter light sleep
        esp_light_sleep_start();
        printf("Woke up from light sleep!\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
