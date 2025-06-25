#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "driver/dac.h"
#include "esp_timer.h"



#define DAC_CH DAC_CHANNEL_1
#define ADC_CH ADC1_CHANNEL_6
#define WAVE_SAMPLES 64

const uint8_t sine_table[WAVE_SAMPLES] = {
  128,140,153,165,177,188,198,208,
  217,224,231,236,240,243,245,246,
  245,243,240,236,231,224,217,208,
  198,188,177,165,153,140,128,115,
  102, 90, 78, 67, 57, 47, 38, 31,
   24, 19, 15, 12, 10,  9, 10, 12,
   15, 19, 24, 31, 38, 47, 57, 67,
   78, 90,102,115
};

volatile int index = 0;

void timer_callback(void* arg)
{
    dac_output_voltage(DAC_CH, sine_table[index]);
    esp_rom_delay_us(30); 
    int raw1 = adc1_get_raw(ADC_CH);
    esp_rom_delay_us(5);
    int raw2 = adc1_get_raw(ADC_CH);
    float voltage = ((raw1 + raw2) / 2.0) / 4095.0 * 3.3;
    printf("voltage: %.2f\n", voltage);

    index = (index + 1) % WAVE_SAMPLES;
}

void app_main(void)
{
    dac_output_enable(DAC_CH);
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC_CH, ADC_ATTEN_DB_12);

    float freq = 50.0;
    int interval_us = (1e6 / (freq * WAVE_SAMPLES));

    const esp_timer_create_args_t timer_args = {
        .callback = &timer_callback,
        .name = "sine_gen_timer"
    };

    esp_timer_handle_t timer;
    esp_timer_create(&timer_args, &timer);
    esp_timer_start_periodic(timer, interval_us);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
