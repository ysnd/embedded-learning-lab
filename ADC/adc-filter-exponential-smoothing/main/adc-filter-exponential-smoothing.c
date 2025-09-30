#include <stdint.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"


#define POT ADC1_CHANNEL_5

void app_main(void)
{
    float filtered = 0;
    float alpha = 0.2;

    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(POT, ADC_ATTEN_DB_12);

    while (1) {
        uint32_t raw = adc1_get_raw(POT);
        filtered = alpha * raw + (1 - alpha) * filtered;
        printf("Raw: %ld | Filtered: %.2f\n", raw, filtered);
        vTaskDelay(pdMS_TO_TICKS(200));
    
    }
}

