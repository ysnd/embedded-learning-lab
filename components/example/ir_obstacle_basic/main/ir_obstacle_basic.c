#include <stdio.h>
#include "ir_obstacle.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

ir_obstacle_t sensor_kanan, sensor_kiri;

void app_main(void)
{
    ir_obstacle_init(&sensor_kanan, 18);
    ir_obstacle_init(&sensor_kiri, 0);

    while (1) {
        if (ir_obstacle_read(&sensor_kanan)) {
            printf("Kanan Ada Halangan\n");
        }
        if (ir_obstacle_read(&sensor_kiri)) {
            printf("Kiri Ada Halangan\n");
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
