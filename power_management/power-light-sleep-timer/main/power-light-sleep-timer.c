#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_sleep.h"

void app_main(void)
{
    printf("Start Light Sleep Example")

    while (1) {
        printf("Going to light sleep (5s or buttn press)...\n");

        //enabke wakeup_soources
        esp_sleep_enable_timer_wakeup(10000000); // 5 s  
        //enter light sleep
        esp_light_sleep_start();
        printf("Woke up from light sleep!\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
