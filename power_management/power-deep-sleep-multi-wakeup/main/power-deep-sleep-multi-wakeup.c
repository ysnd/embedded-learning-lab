#include <stdint.h>
#include <stdio.h>
#include "driver/gpio.h"
#include "driver/touch_sensor_common.h"
#include "driver/touch_sensor_legacy.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "esp_sleep.h"

#define LED GPIO_NUM_2
#define BTN GPIO_NUM_0
#define TOUCH TOUCH_PAD_NUM0
#define DEBOUNCE_MS 50

void led_blink_pattern(int times, int delay_ms) {
    for (int i=0; i<times; i++) {
        gpio_set_level(LED, 1);
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
        gpio_set_level(LED, 0);
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}

void led_task(void *pvParameters) {
    int wake_source = *(int*)pvParameters;
    switch (wake_source) {
        case ESP_SLEEP_WAKEUP_TIMER:
            led_blink_pattern(3, 200);
            break;
        case ESP_SLEEP_WAKEUP_EXT0:
            led_blink_pattern(5, 100);
            break;
        default:
            led_blink_pattern(1, 500);
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
    touch_pad_init();
    touch_pad_config(TOUCH, 0);

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

    xTaskCreate(led_task, "led_task", 2048, (void*)&cause, 5, NULL);
    uint16_t touch_val=0;
    touch_pad_read(TOUCH, &touch_val);
    if (touch_val < 40) {
        printf("Toouch detected after wake up\n");
        led_blink_pattern(2, 400);
    }
    //atur deep sleep timer
    printf("Entering deep sleep for 10s... or button press...\n");
    esp_sleep_enable_timer_wakeup(10000000);
    esp_sleep_enable_ext0_wakeup(BTN, 0);

    vTaskDelay(pdMS_TO_TICKS(DEBOUNCE_MS));

    esp_deep_sleep_start();
}



