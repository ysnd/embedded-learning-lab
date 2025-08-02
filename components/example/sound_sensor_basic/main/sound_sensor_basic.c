#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sound_sensor.h"

sound_sensor_t sensor_pin;

void app_main(void)
{
    sound_sensor_init(&sensor_pin, GPIO_NUM_18);

    while (1) {
        if (sound_sensor_is_detected(&sensor_pin)) {
            printf("Suara Terdeteksi!\n");
        }
        else {
            printf("Suara Tidak Terdeteksi\n");
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}  
