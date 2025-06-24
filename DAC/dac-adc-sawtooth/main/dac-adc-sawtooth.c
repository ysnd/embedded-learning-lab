#include <stdint.h>
#include <stdio.h>
#include "driver/adc_types_legacy.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "driver/dac.h"
#include "driver/adc.h"
#include "hal/adc_types.h"
#include "hal/dac_types.h"

#define DAC_CH DAC_CHANNEL_1
#define ADC_CH ADC1_CHANNEL_6

void app_main(void)
{
    dac_output_enable(DAC_CH);
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC_CH, ADC_ATTEN_DB_12);

    uint8_t val = 0;

    while (1) {
        dac_output_voltage(DAC_CH, val);

        int adc_raw = adc1_get_raw(ADC_CH);
        float voltage = (adc_raw/4095.0)*3.3;
        printf("voltage: %.2f\n",voltage);

        val +=5;
        if (val >= 255) {
            val = 0;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
