#include <stdint.h>
#include <stdio.h> 
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "esp_timer.h"


void app_main(void)
{
    while (1) {
        int64_t us = esp_timer_get_time();
        int total_s = us / 1000000;
    
        int hours = total_s / 3600;
        int minutes = (total_s % 3600) / 60;
        int seconds = total_s % 60;

        printf("Uptime: %02d:%02d:%02d\n", hours, minutes, seconds);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
