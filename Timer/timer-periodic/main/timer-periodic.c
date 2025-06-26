#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_log.h"

static int count = 0;
void periodic_callback(void* arg) {
    count++;
    ESP_LOGI("TIMER", "Periodic timer fired! Count: %d", count);
}

void app_main(void)
{
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &periodic_callback,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "periodic"
    };
    esp_timer_handle_t periodic_timer;
    esp_timer_create(&periodic_timer_args, &periodic_timer);
    ESP_LOGI("TIMER", "Starting periodic timer (interval 1 detik)...");
    esp_timer_start_periodic(periodic_timer, 1000000);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(5000));
        ESP_LOGI("TIMER", "Running main loop...");
    
    }
}
