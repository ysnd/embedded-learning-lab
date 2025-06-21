#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "driver/ledc.h"


#define SERVO 5
#define POT ADC1_CHANNEL_6

int adc_to_angle(int raw_adc) {
    return (raw_adc *180) / 4095;
}

uint32_t angle_to_duty(int angle) {
    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;    
    return (uint32_t)(13.0 +((float)angle / 180.0) * (26.0-13.0));
    }

void app_main(void)
{
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(POT, ADC_ATTEN_DB_12);

    ledc_timer_config_t timer = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num  = LEDC_TIMER_0,
        .duty_resolution    = LEDC_TIMER_8_BIT,
        .freq_hz    = 50,
        .clk_cfg    = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer);

    ledc_channel_config_t channel = {
        .gpio_num   = SERVO,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel    = LEDC_CHANNEL_0,
        .duty   = 0,
        .timer_sel  = LEDC_TIMER_0,
        .hpoint = 0 
    };
    ledc_channel_config(&channel);

    while (1) {
        int raw = adc1_get_raw(POT);
        int angle = adc_to_angle(raw);
        uint32_t duty = angle_to_duty(angle);
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, duty);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
        printf("ADC: %d -> Duty: %lu -> Angle: %d\n", raw, duty, angle);
        vTaskDelay(pdMS_TO_TICKS(200));

    
    }
}
