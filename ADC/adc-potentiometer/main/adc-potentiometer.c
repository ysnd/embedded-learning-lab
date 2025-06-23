#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"


#define POT ADC1_CHANNEL_6

void app_main(void)
{
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(POT, ADC_ATTEN_DB_11);

    while (1) {
        int raw = adc1_get_raw(POT);
        printf("ADC Value: %d\n", raw);
        vTaskDelay(pdMS_TO_TICKS(200));
    
    }
}
