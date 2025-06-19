#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"


#define SERVO 5
#define SERVO_FREQ 50

void app_main(void)
{
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_8_BIT,
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
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 13);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
        printf("Sudut 0\n");
        vTaskDelay(pdMS_TO_TICKS(1000));

        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 19);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
        printf("Sudut 90\n");
        vTaskDelay(pdMS_TO_TICKS(1000));

        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 26);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
        printf("Sudut 180\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
