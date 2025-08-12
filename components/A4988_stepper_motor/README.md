# A4988 Servo Motor Driver Component (ESP-IDF)

This component is a basic driver for controlling motor stepper with A4988 using ESP-IDF.

## Features
- Control stepper motor with A4988 driver
- Support Full-step, Half-step, Quarter-step, Eighth-step and Sixteenth-step
- Adjustable direction (CW/CCW)
- Adjustable speed via delay parameter
- Simple and reusable

## How to Use
1. initialize the stepper :

```c
#include "A4988.h"

stepper_config_t conf = {
    .step = 16,       // Pin STEP
    .dir  = 4,        // Pin DIR
    .en   = 5,        // Pin ENABLE
    .ms1  = 19,       // Pin MS1 (use STEP_PIN_UNUSED if not used microstepping)
    .ms2  = 18,       // Pin MS2
    .ms3  = 23        // Pin MS3
};

stepper_init(&conf);

```

2. Move the stepper :

```c
// Move 2048 steps CW in FULL STEP mode, delay 800µs
stepper_move(2048, 800, FULL_STEP_MODE);

// Move 2048 steps CCW in HALF STEP mode, delay 800µs
stepper_move(-2048, 800, HALF_STEP_MODE);
```
## References
- `void stepper_init(const stepper_config_t *conf);`
- `void stepper_move(int steps, uint32_t delay_us, stepper_mode_t mode);`

## Parameters

| Parameter | Description|
| --- | --- |
| `steps` | Number of steps to move ( `steps` = Clockwise, `-steps` = Counterclockwise) |
| `delay_us` | delay between steps in microseconds (smaller=faster) |
| `mode` | `FULL_STEP_MODE`, `HALF_STEP_MODE`, `QUARTER_STEP_MODE`, `EIGHTH_STEP_MODE`, `SIXTEENTH_STEP_MODE` |

## Notes
- if microstepping not used, set pin ms1, ms2, and ms3 to `STEP_PIN_UNUSED` the driver default mode to `FULL_STEP_MODE` because ms1-ms3 are not conected with any logic (floating) or pulled `LOW`
- ensure the A4988 current limit is set to the motor used spesifications before use.
- see example usage in `../components/example/A4988/`
- This component is a basic library for A4988 driver stepper motor, created as part of the learning process on how to build modular components in ESP-IDF.




