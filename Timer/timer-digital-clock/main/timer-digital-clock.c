#include <stdio.h>
#include "esp_timer.h"

int hours = 0, minutes = 0, seconds = 0;

void tick(void* arg) {
    seconds++;
    if (seconds >=60) {
        seconds = 0;
        minutes++;
        if (minutes >=60) {
            hours = 0;
            hours = (hours + 1) % 24;
        }
    }
    printf("Current time : %02d:%02d:%02d\n", hours, minutes, seconds);
}

void app_main(void)
{
    esp_timer_handle_t clock_timer;

    const esp_timer_create_args_t timer_args = {
        .callback = &tick,
        .name = "Clock_TIme"
    };
    esp_timer_create(&timer_args, &clock_timer);
    esp_timer_start_periodic(clock_timer, 1000000);
}
