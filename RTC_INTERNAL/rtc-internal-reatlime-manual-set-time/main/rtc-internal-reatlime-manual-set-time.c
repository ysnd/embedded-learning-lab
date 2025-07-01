#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"

void app_main(void)
{
    struct tm t = {
        .tm_year = 2025 - 1900,
        .tm_mon = 7 - 1,
        .tm_mday = 1,
        .tm_hour = 11,
        .tm_min = 30,
        .tm_sec = 0
    };
    time_t now = mktime(&t);
    struct timeval tv = { .tv_sec= now, .tv_usec = 0 };
    settimeofday(&tv, NULL);

    while (1) {
        struct timeval now;
        gettimeofday(&now, NULL);

        time_t seconds = now.tv_sec;
        struct tm timeinfo;
        localtime_r(&seconds, &timeinfo);

        char buffer[64];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
        printf("Current time : %s\n", buffer);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
