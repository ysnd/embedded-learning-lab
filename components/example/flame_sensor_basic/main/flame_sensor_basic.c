#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "flame_sensor.h"

flame_sensor_t sensor;

void app_main(void)
{
    flame_sensor_int(&sensor, GPIO_NUM_18);

    while (1) {
        if (flame_sensor_is_detected(&sensor)) {
            printf("Api Terdeteksi!\n"); 
        }
        else {
            printf("Tidak Ada Api\n");
        }
        vTaskDelay(pdMS_TO_TICKS(300));
    }
}
