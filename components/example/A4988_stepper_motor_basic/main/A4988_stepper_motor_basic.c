#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "A4988.h"

#define PULSE_US 800

void app_main(void)
{
    const stepper_config_t pin = {
        .step = 16,
        .dir = 4,
        .en = 5,
        .ms1 = 19,
        .ms2 = 18,
        .ms3 = 23
    };
    stepper_init(&pin);

    while (1) {
        stepper_move(2048, 800, FULL_STEP_MODE);
        printf("FULL-STEP CW\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
        stepper_move(-2048, 800, FULL_STEP_MODE);
        printf("FULL-STEP CCW\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
        stepper_move(2048, 800, HALF_STEP_MODE);
        printf("HALF-STEP CW\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
        stepper_move(-2048, 800, HALF_STEP_MODE);
        printf("HALF-STEP CW\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
        stepper_move(2048, 800, QUARTER_STEP_MODE);
        printf("QUARTER-STEP CW\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
        stepper_move(-2048, 800, QUARTER_STEP_MODE);
        printf("QUARTER-STEP CW\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
        stepper_move(2048, 800, EIGHTH_STEP_MODE);
        printf("EIGHTH-STEP CW\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
        stepper_move(-2048, 800, EIGHTH_STEP_MODE);
        printf("EIGHTH-STEP CW\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
        stepper_move(2048, 800, SIXTEENTH_STEP_MODE);
        printf("SIXTEENTH-STEP CW\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
        stepper_move(-2048, 800, SIXTEENTH_STEP_MODE);
        printf("SIXTEENTH-STEP CW\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
