#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"


void app_main(void) {
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_12);
    adc1_config_channel_atten(ADC1_CHANNEL_3, ADC_ATTEN_DB_12);

    while (1) {
        int val_ch0 = adc1_get_raw(ADC1_CHANNEL_0);
        int val_ch3 = adc1_get_raw(ADC1_CHANNEL_3);
        int hall_val = val_ch0 - val_ch3;

        printf("CH0=%d, CH3=%d, HALL_RAW=%d\n",val_ch0, val_ch3, hall_val);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
