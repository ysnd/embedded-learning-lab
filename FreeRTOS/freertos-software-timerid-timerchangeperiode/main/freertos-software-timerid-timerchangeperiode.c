#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "driver/gpio.h"

#define LED 2

TimerHandle_t timer_led;
TimerHandle_t timer_print;
TimerHandle_t timer_period_changer;

void led_timer_callback(TimerHandle_t xTimer) {
    static bool led_state = false;
    led_state = !led_state;
    gpio_set_level(LED, led_state);
    printf("LED toggled\n");
}

void print_timer_callback(TimerHandle_t xTimer) {
    const char* name = (const char*) pvTimerGetTimerID(xTimer);
    printf("Callback from timer: %s\n", name);
}

void change_period_callback(TimerHandle_t xTimer) {
    printf("Changing TimerB period to 500ms\n");
    xTimerChangePeriod(timer_print, pdMS_TO_TICKS(500), 0);
}

void app_main(void)
{
    gpio_reset_pin(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);

    timer_led = xTimerCreate("LED timer", pdMS_TO_TICKS(1000), pdTRUE, NULL, led_timer_callback);
    timer_print = xTimerCreate("Print Timer", pdMS_TO_TICKS(2000), pdTRUE, (void*)"TimerB", print_timer_callback);
    timer_period_changer = xTimerCreate("Timer Changer", pdMS_TO_TICKS(10000), pdFALSE, NULL, change_period_callback);
    xTimerStart(timer_led, 0);
    xTimerStart(timer_print, 0);
    xTimerStart(timer_period_changer, 0);

}
