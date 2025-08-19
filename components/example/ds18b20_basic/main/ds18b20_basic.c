#include <stdio.h>
#include "ds18b20.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"

#define sensor_pin 4

void app_main(void)
{
    ds18b20_init(sensor_pin);

    while (1) {
        float temperatur = ds18b20_read_temp(sensor_pin);
        printf("Suhu : %.2f Celcius\n", temperatur);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

}
