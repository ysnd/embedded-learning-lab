#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "hal/gpio_types.h"

#define flame_sensor 18


void app_main(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << flame_sensor),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = 0,
        .pull_down_en = 0,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);

    while (1) {
        bool flame = gpio_get_level(flame_sensor);
        if ( flame == 0 ) {
            printf("flame detected!\n");
        }
        else {
            printf("flame not detected\n");
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
