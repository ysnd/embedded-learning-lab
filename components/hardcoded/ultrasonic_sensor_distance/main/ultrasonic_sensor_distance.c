#include <stdint.h>
#include <stdio.h>
#include "esp_attr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"

#define TRG GPIO_NUM_18
#define ECH GPIO_NUM_5

void ultrasonic_init()
{
    gpio_reset_pin(TRG);
    gpio_reset_pin(ECH);
    gpio_set_direction(TRG, GPIO_MODE_OUTPUT);
    gpio_set_direction(ECH, GPIO_MODE_INPUT);
    gpio_set_level(TRG, 0);

}

float ultrasonic_measure() {

    gpio_set_level(TRG, 0);
    esp_rom_delay_us(2);
    gpio_set_level(TRG, 1);
    esp_rom_delay_us(10);
    gpio_set_level(TRG, 0);

    int64_t start_time = esp_timer_get_time();
    while (gpio_get_level(ECH) ==0) {
        if ((esp_timer_get_time() - start_time) > 100000) return -1;
    }

    int64_t echo_start = esp_timer_get_time();
    while (gpio_get_level(ECH) ==1) {
        if ((esp_timer_get_time() - echo_start) > 100000) return -1;       
    }

    int64_t echo_end = esp_timer_get_time();
    int64_t pulse_duration = echo_end - echo_start;

    float distance_cm = pulse_duration * 0.0343 / 2.0;
    return distance_cm;
}
    


void app_main(void) {
    ultrasonic_init();
    while (1) {
        float distance = ultrasonic_measure();
        if (distance>0) {
            printf("Jarak: %.2f cm\n", distance);
        }
        else {
            printf("Sensor timeout\n");
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
