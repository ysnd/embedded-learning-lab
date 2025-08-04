#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "water_sensor.h"

#define pin 18
water_sensor_t ws;

void app_main(void)
{
    water_sensor_init(&ws, pin);

    while (1) {
        if (water_sensor_is_detected(&ws)) {
            printf("Air terdeteksi!\n");
        }
        else {
            printf("Air Tidak Terdeteksi\n");
        }
        vTaskDelay(pdMS_TO_TICKS(300));
    }
}
