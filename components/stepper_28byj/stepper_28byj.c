#include "stepper_28byj.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdint.h>

static stepper_config_t motor_config;

static const uint8_t half_step_seq[8][4] = {
    {1,0,0,0},
    {1,1,0,0},
    {0,1,0,0},
    {0,1,1,0},
    {0,0,1,0},
    {0,0,1,1},
    {0,0,0,1},
    {1,0,0,1}
};

static const uint8_t full_step_seq[4][4] = {
    {1,1,0,0},
    {0,1,1,0},
    {0,0,1,1},
    {1,0,0,1}
};

void stepper_init(const stepper_config_t *config) {
    motor_config = *config;

    gpio_reset_pin(motor_config.in1);
    gpio_reset_pin(motor_config.in2);
    gpio_reset_pin(motor_config.in3);
    gpio_reset_pin(motor_config.in4);

    gpio_set_direction(motor_config.in1, GPIO_MODE_OUTPUT);
    gpio_set_direction(motor_config.in2, GPIO_MODE_OUTPUT);
    gpio_set_direction(motor_config.in3, GPIO_MODE_OUTPUT);
    gpio_set_direction(motor_config.in4, GPIO_MODE_OUTPUT);

    stepper_stop();
}

void stepper_stop() {
    gpio_set_level(motor_config.in1, 0);
    gpio_set_level(motor_config.in2, 0);
    gpio_set_level(motor_config.in3, 0);
    gpio_set_level(motor_config.in4, 0);
}

void stepper_move(stepper_direction_t direction, uint32_t steps, uint32_t step_delay_ms, stepper_mode_t mode) {
    const uint8_t (*sequence)[4];
    int steps_per_cycle;

    if (mode == STEPPER_MODE_FULL_STEP) {
        sequence = full_step_seq;
        steps_per_cycle = 4;
    } else {
        sequence = half_step_seq;
        steps_per_cycle = 8;
    }

    for (uint32_t i = 0; i < steps; i++) {
        uint32_t step_index = i % steps_per_cycle;
        uint32_t seq_index;
        if (direction == STEPPER_DIR_CW) {
            seq_index = step_index;
        } else {
            seq_index = (steps_per_cycle - 1) -step_index;
        }

        gpio_set_level(motor_config.in1, sequence[seq_index][0]);
        gpio_set_level(motor_config.in2, sequence[seq_index][1]);
        gpio_set_level(motor_config.in3, sequence[seq_index][2]);
        gpio_set_level(motor_config.in4, sequence[seq_index][3]);
    
        vTaskDelay(pdMS_TO_TICKS(step_delay_ms));
    }
    stepper_stop();
}






