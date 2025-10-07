#include <stdio.h>
#include "driver/gpio.h"
#include "driver/touch_sensor_common.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "esp_sleep.h"
#include "driver/touch_pad.h"

#define LED GPIO_NUM_2
#define TOUCH TOUCH_PAD_NUM0

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
    touch_pad_init();
    touch_pad_config(TOUCH_PAD_NUM0, 0);


    //cek sleep wakeup casee
    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
    switch (cause) {
        case ESP_SLEEP_WAKEUP_TIMER:
            printf("Woke up from timer!\n");
            break;
        case ESP_SLEEP_WAKEUP_TOUCHPAD:
            printf("Woke up frm touch pad\n");
            break;
        default:
            printf("Poower on r oother wakeup cause\n"); 
    }

    xTaskCreate(led_task, "led_task", 2048, NULL, 5, NULL);
    
    //atur deep sleep timer
    printf("Entering deep sleep for 10s... or touch sensor...\n");
    esp_sleep_enable_timer_wakeup(10000000);
    esp_sleep_enable_touchpad_wakeup();
    esp_deep_sleep_start();
}



