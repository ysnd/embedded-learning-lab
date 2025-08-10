#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define pir_pin 18

void app_main(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << pir_pin),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = 0,
        .pull_down_en = 0,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);

    while (1) {
        bool gerakan = gpio_get_level(pir_pin);
        if (gerakan ==1) {
            printf("Gerakan terdeteksi!\n");
        }
        else {
            printf("Tidak ada gerakan\n");
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
