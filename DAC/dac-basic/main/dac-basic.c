#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/dac.h"
#include "hal/dac_types.h"



void app_main(void)
{
    dac_output_enable(DAC_CHANNEL_1);

    uint8_t val = 0;
    int8_t step = 5;

    while (1) {
        dac_output_voltage(DAC_CHANNEL_1, val);
        val += step;
        if (val >= 255 || val <=0) {
            step = -step;       }
        vTaskDelay(pdMS_TO_TICKS(50));
        printf("DAC : %ul\n", val);
    }
}
