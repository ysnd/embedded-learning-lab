#include <stdio.h>
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"


#define SERVO GPIO_NUM_5


void servo_init()
{
    ledc_timer_config_t timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = LEDC_TIMER_16_BIT,
        .freq_hz = 50,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer);

    ledc_channel_config_t channel = {
        .gpio_num = SERVO,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0 
    };
    ledc_channel_config(&channel);
}

uint32_t angle_to_duty(int angle) {
    if (angle > 180) angle = 180;
    if (angle < 0) angle = 0;   
    uint32_t max_duty = (1 << LEDC_TIMER_16_BIT) - 1; //6553
    uint32_t min_duty = max_duty * 0.025; //2.5% duty = 0.5 ms
    uint32_t max_duty_val = max_duty * 0.125; // 12.5% duty = 2.5ms
    return min_duty + ((max_duty_val - min_duty) * angle) / 180;
}

void app_main(void) {
    servo_init();
    
    while (1) {
        for (int angle = 0; angle <= 180; angle += 30) {
            uint32_t duty = angle_to_duty(angle);
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
            printf("Sudut: %3d , Duty: %5lu\n", angle, (unsigned long)duty);
            vTaskDelay(pdMS_TO_TICKS(500));
        }
        for (int angle = 180; angle >=0; angle -=30) {
            uint32_t duty = angle_to_duty(angle);
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
            printf("Sudut: %3d , Duty: %5lu\n", angle, (unsigned long)duty);
            vTaskDelay(pdMS_TO_TICKS(500));
        }
    }
}

