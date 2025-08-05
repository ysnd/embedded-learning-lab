#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "ultrasonic_sensor.h"


void app_main(void)
{
    ultrasonic_sensor_init(GPIO_NUM_5, GPIO_NUM_18);

    while (1) {
        float jarak = ultrasonic_sensor_read_cm();
        if (jarak >0) {
            printf("Jarak: %.2f cm\n", jarak);
        }
        else {
            printf("Tidak Ada Objek\n");
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
