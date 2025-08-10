#include <stdint.h>
#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"

#define IR 18

void app_main(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << IR),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);

    while (1) {
        uint8_t val = gpio_get_level(IR);
        if (val == 0) {
            printf("Halangan Terdeteksi\n");
        }
        else {
            printf("Tidak Ada Halangan\n");
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
