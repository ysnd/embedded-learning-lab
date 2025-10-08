#include <stdio.h>
#include "driver/gpio.h"
#include "esp_attr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_sleep.h"

#define LED GPIO_NUM_2
#define BTN GPIO_NUM_0

RTC_DATA_ATTR int wake_count = 0;

void led_blink_task(void *pvParameters) {
    for (int i=0; i<3; i++) {
        gpio_set_level(LED, 1);
        vTaskDelay(pdMS_TO_TICKS(200));
        gpio_set_level(LED, 0);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
    vTaskDelete(NULL);
}

void app_main(void)
{
    gpio_reset_pin(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    gpio_reset_pin(BTN);
    gpio_set_direction(BTN, GPIO_MODE_INPUT);
    gpio_pullup_en(BTN);

    //coount wakeup 
    wake_count++;
    printf("Wake count: %d\n", wake_count);

    xTaskCreate(led_blink_task, "led_task", 2048, NULL, 5, NULL);
    
    //atur deep sleep timer
    printf("Entering ultra_low power hibernation...\n");
    //enable wake-up
    esp_sleep_enable_ext0_wakeup(BTN, 0);
    esp_sleep_enable_timer_wakeup(5000000);

    vTaskDelay(pdMS_TO_TICKS(50));
    esp_deep_sleep_start();
}



