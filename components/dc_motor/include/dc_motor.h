#pragma once

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_err.h"

typedef struct {
    gpio_num_t in1;
    gpio_num_t in2;
    gpio_num_t pwm_pin;
    ledc_channel_t pwm_channel;
} motor_channel_t;

typedef struct {
    motor_channel_t left;
    motor_channel_t right;
    ledc_timer_t pwm_timer;
    uint32_t pwm_freq_hz;
    ledc_timer_bit_t pwm_resolution;
} motor_driver_t;

esp_err_t motor_init(motor_driver_t *driver);

void motor_forward(motor_driver_t *driver, uint32_t speed);
void motor_backward(motor_driver_t *driver, uint32_t speed);
void motor_turn_left(motor_driver_t *driver, uint32_t speed);
void motor_turn_right(motor_driver_t *driver, uint32_t speed);
void motor_stop(motor_driver_t *driver);

void motor_set_speed(motor_driver_t *driver, int left_speed, int right_speed);
