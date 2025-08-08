#include "servo.h"

esp_err_t servo_init(servo_t *servo, gpio_num_t gpio, 
                       ledc_channel_t channel, ledc_timer_t timer)
{
    servo->gpio = gpio;
    servo->channel = channel;
    servo->timer = timer;
    servo->min_us = 500;      // default
    servo->max_us = 2400;     // default
    servo->max_angle = 180;   // default

    ledc_timer_config_t timer_conf = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = servo->timer,
        .duty_resolution = LEDC_TIMER_16_BIT,
        .freq_hz = 50,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer_conf);

    ledc_channel_config_t channel_conf = {
        .gpio_num = servo->gpio,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = servo->channel,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = servo->timer,
        .duty = 0,
        .hpoint = 0
    };
    return ledc_channel_config(&channel_conf);
}

void servo_write(servo_t *servo, uint16_t angle)
{
    if (angle > servo->max_angle) angle = servo->max_angle;

    uint32_t us = servo->min_us + 
                  (servo->max_us - servo->min_us) * angle / servo->max_angle;

    uint32_t duty = (uint32_t)((us * (1 << 16)) / 20000); // 20ms period
    ledc_set_duty(LEDC_LOW_SPEED_MODE, servo->channel, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, servo->channel);
}

