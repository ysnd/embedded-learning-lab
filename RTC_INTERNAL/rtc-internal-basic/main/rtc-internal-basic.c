#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "esp_timer.h"

void app_main(void)
{
    while (1) {
        int64_t now_us = esp_timer_get_time();
        printf("Uptime: %lld microseconds (%.2f detik)\n", now_us, now_us/1000000.0);
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
}
