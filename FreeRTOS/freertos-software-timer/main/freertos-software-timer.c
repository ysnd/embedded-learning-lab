#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "driver/gpio.h"

#define LED 2

void vBlinkTimerCallback(TimerHandle_t xTimer) {
    static bool led_on = false;
    led_on = !led_on;
    gpio_set_level(LED, led_on);
}

void app_main(void)
{
    gpio_reset_pin(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);

    TimerHandle_t blink_timer = xTimerCreate(
            "BlinkTimer", 
            pdMS_TO_TICKS(1000), 
            pdTRUE, 
            NULL, 
            vBlinkTimerCallback
            );
    if (blink_timer != NULL) {
        xTimerStart(blink_timer, 0);
    }
}
