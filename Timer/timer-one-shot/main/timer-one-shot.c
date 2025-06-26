#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_log.h"

void oneshot_callback(void* arg) {
    ESP_LOGI("TIMER", "One-shot timer triggered!");
}

void app_main(void)
{
    const esp_timer_create_args_t oneshot_timer_args = {
        .callback = &oneshot_callback,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "one-shot"
    };

    esp_timer_handle_t oneshot_timer;
    esp_timer_create(&oneshot_timer_args, &oneshot_timer);

    ESP_LOGI("TIMER", "Starting one-shot timer (2 detik...");
    esp_timer_start_once(oneshot_timer, 2000000);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        ESP_LOGI("TIMER", "tungguan...");
    }
}
