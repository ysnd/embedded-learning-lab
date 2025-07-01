#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    while (1) {
        struct timeval tv;
        gettimeofday(&tv, NULL);

        time_t now = tv.tv_sec;
        struct tm timeinfo;
        localtime_r(&now, &timeinfo);
        
        char buffer[64];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);

        printf("Current Time %s\n", buffer);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
