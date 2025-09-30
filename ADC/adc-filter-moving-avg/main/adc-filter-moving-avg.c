#include <stdint.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"


#define POT ADC1_CHANNEL_5
#define SAMPLE_WINDOW 8

void app_main(void)
{
    uint32_t samples[SAMPLE_WINDOW] = {0};
    int index = 0;
    uint32_t sum = 0;

    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(POT, ADC_ATTEN_DB_12);

    while (1) {
        uint32_t raw = adc1_get_raw(POT);
        sum -= samples[index];
        sum += raw;
        samples[index] = raw;
        index = (index + 1) % SAMPLE_WINDOW;
        uint32_t avg = sum / SAMPLE_WINDOW;
        printf("Raw: %ld | Filtered: %ld\n", raw, avg);
        vTaskDelay(pdMS_TO_TICKS(200));
    
    }
}
