#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED 4
#define BUTTON 2

void app_main(void)
{
    gpio_reset_pin(LED);
    gpio_set_direction(BUTTON, GPIO_MODE_INPUT);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    gpio_set_pull_mode(BUTTON, GPIO_PULLUP_ONLY);

    int stable_state = 1;         // tombol idle = HIGH
    int last_reading = 1;
    int debounce_counter = 0;
    const int debounce_threshold = 5; // jumlah pembacaan stabil

    while (1) {
        int current_reading = gpio_get_level(BUTTON);

        if (current_reading == last_reading) {
            if (debounce_counter < debounce_threshold) {
                debounce_counter++;
            }
        } else {
            debounce_counter = 0;  // reset counter saat beda
        }

        if (debounce_counter >= debounce_threshold && current_reading != stable_state) {
            stable_state = current_reading;
            printf("Debounced state: %d\n", stable_state);
            gpio_set_level(LED, stable_state == 0 ? 1 : 0); // tombol ditekan = nyalakan LED
        }

        last_reading = current_reading;
        vTaskDelay(pdMS_TO_TICKS(5)); // polling rate lebih cepat
    }
}
