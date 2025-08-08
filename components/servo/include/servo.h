#ifndef SERVO_H
#define SERVO_H

#include "driver/ledc.h"
#include "esp_err.h"

typedef struct {
    ledc_channel_t channel;
    ledc_timer_t timer;
    gpio_num_t gpio;
    uint16_t min_us;
    uint16_t max_us;
    uint16_t max_angle;
} servo_t;

esp_err_t servo_init(servo_t *servo, gpio_num_t gpio, 
                       ledc_channel_t channel, ledc_timer_t timer);
void servo_write(servo_t *servo, uint16_t angle);

#endif

