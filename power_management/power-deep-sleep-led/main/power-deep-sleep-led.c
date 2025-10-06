#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "esp_sleep.h"

#define LED GPIO_NUM_2
#define BTN GPIO_NUM_0

void led_task(void *pvParameters) {
    while (1) {
        gpio_set_level(LED, 1);
        vTaskDelay(pdMS_TO_TICKS(200));
        gpio_set_level(LED, 0);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void app_main(void)
{
    gpio_reset_pin(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    gpio_reset_pin(BTN);
    gpio_set_direction(BTN, GPIO_MODE_INPUT);
    gpio_pullup_en(BTN);

    //cek sleep wakeup casee
    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
    switch (cause) {
        case ESP_SLEEP_WAKEUP_TIMER:
            printf("Woke up from timer!\n");
            break;
        case ESP_SLEEP_WAKEUP_EXT0:
            printf("Woke up frm GPIO\n");
            break;
        default:
            printf("Poower on r oother wakeup cause\n"); 
    }

    xTaskCreate(led_task, "led_task", 2048, NULL, 5, NULL);
    
    //atur deep sleep timer
    printf("Entering deep sleep for 10s... or button press...\n");
    esp_sleep_enable_timer_wakeup(10000000);
    esp_sleep_enable_ext0_wakeup(BTN, 0);
    esp_deep_sleep_start();
}


