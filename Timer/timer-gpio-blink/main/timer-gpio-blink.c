#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"


#define LED 2
static bool led_state = false;

void led_timer_callback(void* arg) {
    led_state = !led_state;
    gpio_set_level(LED, led_state);
    ESP_LOGI("TIMER", "LED %s", led_state? "ON" : "OFF");
}

void app_main(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << LED),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);

    esp_timer_create_args_t led_timer_args = {
        .callback = &led_timer_callback,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "led_timer"
    };
    esp_timer_handle_t led_timer;
    esp_timer_create(&led_timer_args, &led_timer);
    esp_timer_start_periodic(led_timer, 500000);

    while (1) {
        ESP_LOGI("MAIN", "Main loop berjalan");
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
