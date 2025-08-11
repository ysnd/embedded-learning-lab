#pragma once
#include <stdint.h>
#include "driver/gpio.h"


typedef enum {
    STEPPER_DIR_CW = 1, // Clockwise
    STEPPER_DIR_CCW = 0 // Counter Clockwise
} stepper_direction_t;

typedef enum {
    STEPPER_MODE_HALF_STEP = 0,
    STEPPER_MODE_FULL_STEP = 1
} stepper_mode_t;

typedef struct { 
    gpio_num_t in1;
    gpio_num_t in2;
    gpio_num_t in3;
    gpio_num_t in4;
} stepper_config_t;

void stepper_init(const stepper_config_t *config);
void stepper_move(stepper_direction_t direction, uint32_t steps, uint32_t step_delay_ms, stepper_mode_t mode);
void stepper_stop();

