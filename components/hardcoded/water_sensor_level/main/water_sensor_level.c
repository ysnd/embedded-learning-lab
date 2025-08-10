#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define pin 18

void app_main(void)
{
    gpio_reset_pin(pin);
    gpio_set_direction(pin, GPIO_MODE_INPUT);

    while (1) {
        bool wet = gpio_get_level(pin);
        if (wet) {
            printf("Air Terdeteksi!\n");
        }
        else {
            printf("Tidak ada air\n");
        }
        vTaskDelay(pdMS_TO_TICKS(300));
    }

}
