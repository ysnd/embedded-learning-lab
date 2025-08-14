#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "dht11.h"

void app_main(void)
{
    dht11_init(GPIO_NUM_4);
    dht11_data_t data;

    while (1) {
        if (dht11_read(&data)) {
            printf("Temperature : %.1fC, Humidity : %.1f%%\n", data.temperature, data.humidity);
        
        } else {
            printf("Baca data sensor gagal!\n");
        }
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
