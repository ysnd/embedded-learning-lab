#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"

#define SERVO 5
#define SERVO_MIN_DUTY 51
#define SERVO_MAX_DUTY 102
#define SERVO_FREQ 50

void app_main(void)
{
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .duty_resolution    = LEDC_TIMER_10_BIT,
        .timer_num  = LEDC_TIMER_0,
        .freq_hz    = SERVO_FREQ,
        .clk_cfg    = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {
        .channel    = LEDC_CHANNEL_0,
        .duty   = 0,
        .gpio_num   = SERVO,
        .hpoint = 0,
        .timer_sel  = LEDC_TIMER_0
    };
    ledc_channel_config(&ledc_channel);

while (1) {
    //sweep
    for (int duty = SERVO_MIN_DUTY; duty <= SERVO_MAX_DUTY; duty++) {
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, duty);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    vTaskDelay(pdMS_TO_TICKS(500));

    //balik sweep
    for (int duty = SERVO_MAX_DUTY; duty>= SERVO_MIN_DUTY; duty--) {
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, duty);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    vTaskDelay(pdMS_TO_TICKS(500));

}
}


