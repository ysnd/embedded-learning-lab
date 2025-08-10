#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"


#define PIN GPIO_NUM_18

void hysrf_dual_init() {
    gpio_reset_pin(PIN);
    gpio_set_direction(PIN, GPIO_MODE_INPUT);
}

float hysrf_dual_read_cm() {
    int64_t wait_start = esp_timer_get_time();
    while (gpio_get_level(PIN) ==0) {
        if ((esp_timer_get_time() - wait_start) > 100000) return -1;
    }
    int64_t pulse_start = esp_timer_get_time();
    while (gpio_get_level(PIN) == 1) {
        if ((esp_timer_get_time() - pulse_start) > 100000) return -1;
    }
    int64_t pulse_end = esp_timer_get_time();
    int64_t pulse_duration = pulse_end - pulse_start;

    float distance_cm = pulse_duration * 0.0343 / 2.0;
    return distance_cm;
}

void app_main(void)
{
    hysrf_dual_init();

    while (1) {
        float jarak = hysrf_dual_read_cm();
        if (jarak>0) {
            printf("Jarak (dual mode OUT): %.2f cm\n", jarak);
        }
        else {
            printf("Timeout\n");
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
