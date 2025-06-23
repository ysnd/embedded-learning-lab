#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "driver/ledc.h"
#include "esp_err.h"


#define LED 4
#define LDR ADC1_CHANNEL_6

void app_main(void)
{
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(LDR, ADC_ATTEN_DB_12);
    ledc_timer_config_t timer = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .duty_resolution    = LEDC_TIMER_8_BIT,
        .timer_num  =   LEDC_TIMER_0,
        .freq_hz    =   1000,
        .clk_cfg    =   LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer);

    ledc_channel_config_t channel = {
        .gpio_num   = LED,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel    = LEDC_CHANNEL_0,
        .duty   = 0,
        .timer_sel  = LEDC_TIMER_0,
        .hpoint = 0 
    };
    ledc_channel_config(&channel);

    while (1) {
        int raw = adc1_get_raw(LDR);
        int duty = 255 - (raw * 255) / 4095;
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, duty);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
        printf("ADC: %d -> Duty: %d\n", raw, duty);
        vTaskDelay(pdMS_TO_TICKS(200));

    
    }
}
