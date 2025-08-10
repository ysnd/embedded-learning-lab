#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "driver/gpio.h"


#define sound_pin 18
 

void app_main(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << sound_pin),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = 0,
        .pull_down_en = 0,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
    
    while (1) {
        bool suara = gpio_get_level(sound_pin);
        if (suara == 0) {
            printf("ada suara\n");
        }
        else {
            printf("Tidak Ada suara\n");
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
