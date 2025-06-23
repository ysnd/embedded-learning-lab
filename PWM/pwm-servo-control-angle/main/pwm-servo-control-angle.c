#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"


#define SERVO 5
#define SERVO_FREQ 50

uint32_t angle_to_duty(int angle) {
    if (angle > 180) angle = 180;
    if (angle < 0) angle = 0;

    return (uint32_t)(13 + ((float)angle / 180.0)*(26-13));
}

void app_main(void)
{
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .duty_resolution    = LEDC_TIMER_8_BIT,
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
        int angle = 0;
        uint32_t duty = angle_to_duty(angle);
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, duty);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
        printf("Angle : %d\n", angle);
        vTaskDelay(pdMS_TO_TICKS(1000));
        angle = 90;
        duty = angle_to_duty(angle);
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, duty);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
        printf("Angle : %d\n", angle);
        vTaskDelay(pdMS_TO_TICKS(1000));
        angle = 180;
        duty = angle_to_duty(angle);
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, duty);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
        printf("Angle : %d\n", angle);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
