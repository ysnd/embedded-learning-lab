#include <stdio.h>
#include "driver/mcpwm.h"
#include "driver/mcpwm_types_legacy.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define PWMA 18
#define PWMB 19


void app_main(void)
{
    //configuration pin for output mcpwm
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, PWMA);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, PWMB);
    //config timer mcpwm
    mcpwm_config_t pwm_cfg = {
        .frequency = 1000, //1khz
        .cmpr_a = 0, //first duty 50%
        .cmpr_b = 0, //first duty 50%
        .counter_mode = MCPWM_UP_COUNTER,
        .duty_mode = MCPWM_DUTY_MODE_0, //active high
    };

    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_cfg);

    //set complementary mode    
    printf("Complementary PWM active in A=%d B=%d\n", PWMA, PWMB);
    //complementary
    mcpwm_set_signal_low(MCPWM_TIMER_0, MCPWM_TIMER_0, MCPWM_OPR_B);
    mcpwm_deadtime_enable(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_ACTIVE_HIGH_COMPLIMENT_MODE, 0, 0);

    float duty = 0.0;
    int step = 10;

    //loop change duty cycle from 0-100% and then turn lower again
    while (1) {
        for (duty = 0; duty <=100;duty +=step) {
            mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, duty);
            mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
            printf("Duty = %.0f%%\n",duty);
            vTaskDelay(pdMS_TO_TICKS(300));
        }
    }
}


