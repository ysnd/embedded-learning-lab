#include <stdint.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "esp_timer.h"

void app_main(void)
{
    printf("Start stopwatch!\n");

    int64_t start_time = esp_timer_get_time();
    vTaskDelay(pdMS_TO_TICKS(3789));

    int64_t end_time = esp_timer_get_time();
    int64_t duration = end_time - start_time;

    printf("Elapsed : %lld us (%.3f detik)\n", duration, duration/ 1000000.0);

}
