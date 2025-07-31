#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "pir_sensor.h"

pir_sensor_t pir;


void app_main(void)
{
    pir_sensor_init(&pir, GPIO_NUM_18);

    while (1) {
        if (pir_sensor_is_motion_detected(&pir)) {
            printf("Gerakan Terdeteksi!\n");
        }
        else {
            printf("Tidak ada gerakan\n");
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
