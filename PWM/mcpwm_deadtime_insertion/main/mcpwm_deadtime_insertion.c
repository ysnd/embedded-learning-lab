#include <stdio.h>
#include "driver/mcpwm.h"
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
        .cmpr_a = 60.0, //first duty 50%
        .cmpr_b = 60.0, //first duty 50%
        .counter_mode = MCPWM_UP_COUNTER,
        .duty_mode = MCPWM_DUTY_MODE_0, //active high
    };

    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_cfg);

    //enable Complementary mode with deadtime     
    uint32_t rising_deadtime_ns = 500; //500ns delay if a rising    
    uint32_t falling_deadtime_ns = 500; //500ns delay if a falling 
                                        //
    mcpwm_deadtime_enable(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_ACTIVE_HIGH_COMPLIMENT_MODE, rising_deadtime_ns, falling_deadtime_ns);
    printf("MCPWM Complementary with deadtime active (500ns)\n");


    while (1) {
        printf("PWM running with deadtime: %lu ns\n", rising_deadtime_ns);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}



