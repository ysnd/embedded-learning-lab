#include <stdio.h>
#include "driver/mcpwm.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LED 18


void app_main(void)
{
    //configuration pin for output mcpwm
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, LED);
    //config timer mcpwm
    mcpwm_config_t pwm_cfg = {
        .frequency = 1000, //1khz
        .cmpr_a = 0, //first duty 0%
        .cmpr_b = 0, //not used
        .counter_mode = MCPWM_UP_COUNTER,
        .duty_mode = MCPWM_DUTY_MODE_0, //active high
    };

    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_cfg);
    printf("MCPWM Initialized on GPIO %d\n", LED);

    float duty = 0.0;
    int step = 10;

    //loop change duty cycle from 0-100% and then turn lower again
    while (1) {
        for (duty = 0; duty <=100;duty +=step) {
            mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, duty);
            mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
            printf("Duty = %.0f%%\n",duty);
            vTaskDelay(pdMS_TO_TICKS(200));
        }
        for (duty = 100; duty>= 0; duty -= step) {
            mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, duty);
            mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
            printf("Duty = %.0f%%\n", duty);
            vTaskDelay(pdMS_TO_TICKS(200));
        }
    }
}

