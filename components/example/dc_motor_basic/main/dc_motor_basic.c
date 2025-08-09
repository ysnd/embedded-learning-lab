#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "dc_motor.h"

void app_main(void)
{
    motor_driver_t driver = {
        .left  = { .in1 = 17, .in2 = 5, .pwm_pin = 18, .pwm_channel = LEDC_CHANNEL_0 },
        .right = { .in1 = 2, .in2 = 16, .pwm_pin = 4, .pwm_channel = LEDC_CHANNEL_1 },
        .pwm_timer      = LEDC_TIMER_0,
        .pwm_freq_hz    = 1000,
        .pwm_resolution = LEDC_TIMER_8_BIT
    };

    motor_init(&driver);
    while (1) {
        motor_forward(&driver, 200);
        printf("Maju");
        vTaskDelay(pdMS_TO_TICKS(2000));
        motor_stop(&driver);
        vTaskDelay(pdMS_TO_TICKS(1000));
        motor_backward(&driver, 253);
        printf("Mundur");
        vTaskDelay(pdMS_TO_TICKS(2000));
        motor_stop(&driver);
        vTaskDelay(pdMS_TO_TICKS(1000));
        motor_turn_left(&driver, 170);
        printf("Kiri");
        vTaskDelay(pdMS_TO_TICKS(2000));
        motor_stop(&driver);
        vTaskDelay(pdMS_TO_TICKS(500));
        motor_turn_right(&driver, 224);
        printf("Kanan");
        vTaskDelay(pdMS_TO_TICKS(2000));
        motor_stop(&driver);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
