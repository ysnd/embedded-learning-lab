#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "hal/ledc_types.h"
#include "soc/clk_tree_defs.h"

#define IN1 2
#define IN2 16
#define IN3 17
#define IN4 5
#define PWMR 4
#define PWML 18

#define PWM_RES LEDC_TIMER_8_BIT
#define DUTY_MAX ((1<<PWM_RES)-1)

void motor_init() {
    gpio_config_t io = {
        .pin_bit_mask = (1ULL << IN1) |
                        (1ULL << IN2) |
                        (1ULL << IN3) |
                        (1ULL << IN4),
        .mode = GPIO_MODE_OUTPUT
    };
    gpio_config(&io);

    ledc_timer_config_t timer = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = PWM_RES,
        .freq_hz = 1000,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer);

    ledc_channel_config_t ch_left = {
        .gpio_num = PWML,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .duty = LEDC_TIMER_0
    };
    ledc_channel_config(&ch_left);

    ledc_channel_config_t ch_right = {
        .gpio_num = PWMR,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel = LEDC_CHANNEL_1,
        .timer_sel = LEDC_TIMER_0,
        .duty = LEDC_TIMER_0
    };
    ledc_channel_config(&ch_right);

}

static void motor_set_right(int16_t speed) {
    bool fwd = speed >=0;
    gpio_set_level(IN1, fwd ? 1 : 0);
    gpio_set_level(IN2, fwd ? 0 : 1);
    uint32_t duty = abs(speed);
    if (duty > DUTY_MAX) duty = DUTY_MAX;
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, duty);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);
}

static void motor_set_left(int16_t speed) {
    bool fwd = speed >=0;
    gpio_set_level(IN3, fwd ? 1 : 0);
    gpio_set_level(IN4, fwd ? 0 : 1);
    uint32_t duty = abs(speed);
    if (duty > DUTY_MAX) duty = DUTY_MAX;
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, duty);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
}

static void motor_stop(void) {
    motor_set_left(0);
    motor_set_right(0);
}


    



void app_main(void)
{
    motor_init();
    while (1) {
        motor_set_right(200);
        motor_set_left(200);
        vTaskDelay(pdMS_TO_TICKS(2000));
        motor_stop();
        vTaskDelay(pdMS_TO_TICKS(500));
        motor_set_right(-200);
        motor_set_left(-200);
        vTaskDelay(pdMS_TO_TICKS(2000));
        motor_stop();
        motor_set_right(200);
        motor_set_left(-200);
        vTaskDelay(pdMS_TO_TICKS(2000));
        motor_stop();
        vTaskDelay(pdMS_TO_TICKS(500));
        motor_set_right(-200);
        motor_set_left(200);
        vTaskDelay(pdMS_TO_TICKS(2000));
        motor_stop();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
