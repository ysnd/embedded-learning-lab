#include <stdio.h>
#include "freertos/projdefs.h"
#include "servo.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

servo_t servo;

void app_main(void)
{
    servo_init(&servo, GPIO_NUM_5, LEDC_CHANNEL_0, LEDC_TIMER_0);

    while (1) {
        for (int i = 0; i <= 180; i +=30) {
            servo_write(&servo, i);
            vTaskDelay(pdMS_TO_TICKS(500));
        }
        for (int i = 180; i >=0; i -=30) {
            servo_write(&servo, i);
            vTaskDelay(pdMS_TO_TICKS(500));
        }
        vTaskDelay(pdMS_TO_TICKS(500));

    }
}

