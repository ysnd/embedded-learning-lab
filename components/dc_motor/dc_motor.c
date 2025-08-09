#include "dc_motor.h"
#include <stdlib.h>
#include "driver/gpio.h"

static void motor_set_direction(motor_channel_t *motor, bool forward) {
    gpio_set_level(motor->in1, forward ? 1 : 0);
    gpio_set_level(motor->in2, forward ? 0 : 1);
}

static void motor_set_duty(motor_channel_t *motor, ledc_timer_bit_t resolution, uint32_t speed) {
    uint32_t duty_max = (1 << resolution) -1;
    if (speed > duty_max) speed = duty_max;
    ledc_set_duty(LEDC_LOW_SPEED_MODE, motor->pwm_channel, speed);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, motor->pwm_channel);
}

esp_err_t motor_init(motor_driver_t *driver) {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << driver->left.in1)	| 
						(1ULL << driver->left.in2)	|
						(1ULL << driver->right.in1)	| 
						(1ULL << driver->right.in2),
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    ledc_timer_config_t t_conf = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = driver->pwm_timer,
        .duty_resolution = driver->pwm_resolution,
        .freq_hz = driver->pwm_freq_hz,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&t_conf));

    ledc_channel_config_t ch_left = {
        .gpio_num = driver->left.pwm_pin,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = driver->left.pwm_channel,
        .timer_sel = driver->pwm_timer,
        .duty = 0,
        .hpoint = 0,
        .flags.output_invert = false
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ch_left));
    
    ledc_channel_config_t ch_right = {
        .channel    = driver->right.pwm_channel,
        .duty       = 0,
        .gpio_num   = driver->right.pwm_pin,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .hpoint     = 0,
        .timer_sel  = driver->pwm_timer
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ch_right));
    return ESP_OK;
}

void motor_backward(motor_driver_t *driver, uint32_t speed) {
    motor_set_direction(&driver->left, true);
    motor_set_direction(&driver->right, true);
    motor_set_duty(&driver->left, driver->pwm_resolution, speed);
    motor_set_duty(&driver->right, driver->pwm_resolution, speed);
}

 void motor_forward(motor_driver_t *driver, uint32_t speed) {
    motor_set_direction(&driver->left, false);
    motor_set_direction(&driver->right, false);
    motor_set_duty(&driver->left, driver->pwm_resolution, speed);
    motor_set_duty(&driver->right, driver->pwm_resolution, speed);
}

void motor_turn_left(motor_driver_t *driver, uint32_t speed) {
    motor_set_direction(&driver->left, true);
    motor_set_direction(&driver->right, false);
    motor_set_duty(&driver->left, driver->pwm_resolution, speed);
    motor_set_duty(&driver->right, driver->pwm_resolution, speed);
}

void motor_turn_right(motor_driver_t *driver, uint32_t speed) {
    motor_set_direction(&driver->left, false);
    motor_set_direction(&driver->right, true);
    motor_set_duty(&driver->left, driver->pwm_resolution, speed);
    motor_set_duty(&driver->right, driver->pwm_resolution, speed);
}

void motor_stop(motor_driver_t *driver) {
    motor_set_duty(&driver->left, driver->pwm_resolution, 0);
    motor_set_duty(&driver->right, driver->pwm_resolution, 0);
}

void motor_set_speed(motor_driver_t *driver, int left_speed, int right_speed) {
    bool left_forward = left_speed >= 0;
    bool right_forward = right_speed >= 0;

    motor_set_direction(&driver->left, left_forward);
    motor_set_direction(&driver->right, right_forward);

    motor_set_duty(&driver->left, driver->pwm_resolution, abs(left_speed));
    motor_set_duty(&driver->right, driver->pwm_resolution, abs(right_speed));
}

