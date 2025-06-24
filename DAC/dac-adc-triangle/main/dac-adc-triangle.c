#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "driver/dac.h"



#define ADC_IN ADC1_CHANNEL_6
#define DAC_OUT DAC_CHANNEL_1

void app_main(void)
{
    dac_output_enable(DAC_OUT);

    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC_IN, ADC_ATTEN_DB_12);

    uint8_t dac_val = 0;
    int8_t step = 5;

    while (1) {
        dac_output_voltage(DAC_OUT, dac_val);
        int adc_raw = adc1_get_raw(ADC_IN);
        float voltage = ( adc_raw / 4095.0 ) * 3.3;
        printf("voltage: %.2f\n", voltage);

        dac_val += step;
        if (dac_val >= 255 || dac_val <= 0 ) {
            step = -step;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
