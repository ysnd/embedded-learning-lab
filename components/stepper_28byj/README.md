# Stepper 28BYJ-48 Component (ESP-IDF)

This component is a basic driver for controlling the 28BYJ-48 motor using ESP-IDF.

## Features
- Control 28BYJ-48 stepper motor with ULN2003 driver
- Support full-step and half-step sequences
- Adjustable direction (CW/CCW)
- Adjustable speed via delay parameter
- Simple and reusable

## How to Use
1. initialize the stepper :

```c
#include "stepper_28byj.h"

stepper_config_t conf = {
    .in1 = 5,
    .in2 = 4,
    .in3 = 16,
    .in4 = 17
};

stepper_init(&conf);
```

2. Move the stepper :

```c
stepper_move(STEPPER_DIR_CW, 4096, 10, STEPPER_MODE_HALF_STEP); //CW, 4096 steps, half-step
stepper_move(STEPPER_DIR_CCW, 2048, 15, STEPPER_MODE_FULL_STEP); //CCW, 2048 steps, full-step
```
## References
- `void stepper_init(stepper_config_t *conf);`
- `void stepper_move(stepper_direction_t dir, uint32_t steps, uint32_t speed, stepper_mode_t mode);`

# Parameters

| Parameter | Description|
| --- | --- |
| `dir` | `STEPPER_DIR_CW` (Clockwise) or `STEPPER_DIR_CCW` (Counterclockwise) |
| `steps` | Number of steps to move |
| `speed` | delay between steps in milliseconds (smaller=faster) |
| `mode` | `STEPPER_MODE_FULL_STEP` or `STEPPER_MODE_HALF_STEP` |

## Notes
- 4096 half-steps = 1 full rotation
- 2048 full-steps = 1 full rotation
- see example usage in `../components/example/stepper_28byj`
- This component is a basic library for control stepper motor 28BYJ-48, created as part of the learning process on how to build modular components in ESP-IDF.



