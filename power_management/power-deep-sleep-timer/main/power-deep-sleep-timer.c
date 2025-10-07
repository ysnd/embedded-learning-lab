#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_sleep.h"

#define LED GPIO_NUM_2

void app_main(void)
{
    gpio_reset_pin(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);

    //cek sleep wakeup casee
    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
    switch (cause) {
        case ESP_SLEEP_WAKEUP_TIMER:
            printf("Woke up from timer!\n");
            break;
        default:
            printf("Poower on r oother wakeup cause\n"); 
    }

    // led indicatr
    gpio_set_level(LED, 1);
    vTaskDelay(pdMS_TO_TICKS(500));
    gpio_set_level(LED, 0);
    //atur deep sleep timer
    printf("Entering deep sleep for 10s...\n");
    esp_sleep_enable_timer_wakeup(10000000);
    esp_deep_sleep_start();
}

