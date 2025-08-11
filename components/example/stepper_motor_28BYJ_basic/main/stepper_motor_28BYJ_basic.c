#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "stepper_28byj.h"

void app_main(void)
{
    stepper_config_t conf = {
        .in1 = 5,
        .in2 = 4,
        .in3 = 16,
        .in4 = 17
    };

    stepper_init(&conf);

    while (1) {
        stepper_move(STEPPER_DIR_CW, 4096, 10, STEPPER_MODE_HALF_STEP);
        printf("HALF-STEP Cepat\n");
        vTaskDelay(1000/portTICK_PERIOD_MS);
        stepper_move(STEPPER_DIR_CCW, 4096, 15, STEPPER_MODE_HALF_STEP);
        printf("HALF-STEP Lambat\n");
        vTaskDelay(1000/portTICK_PERIOD_MS);
        stepper_move(STEPPER_DIR_CW, 2048, 10, STEPPER_MODE_FULL_STEP);
        printf("FULL-STEP Cepat\n");
        vTaskDelay(1000/portTICK_PERIOD_MS);
        stepper_move(STEPPER_DIR_CCW, 2048, 10, STEPPER_MODE_FULL_STEP);
        printf("FULL-STEP Lambat\n");
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}
