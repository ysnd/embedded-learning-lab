#pragma once

#include <stdint.h>
#include "driver/gpio.h"

#define STEP_PIN_UNUSED 0xFF

typedef  enum {
    FULL_STEP_MODE = 0,
    HALF_STEP_MODE,
    QUARTER_STEP_MODE,
    EIGHTH_STEP_MODE,
    SIXTEENTH_STEP_MODE
} stepper_mode_t;

typedef struct {
    uint8_t step;
    uint8_t  dir;
    uint8_t  en;
    uint8_t  ms1;
    uint8_t  ms2;
    uint8_t  ms3;
} stepper_config_t;

void stepper_init(const stepper_config_t *config);
void stepper_set_mode(stepper_mode_t mode);
void stepper_move(int32_t steps, uint32_t pulse_us, stepper_mode_t mode);


