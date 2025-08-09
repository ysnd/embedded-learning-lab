# H-Bridge DC Motor Component (ESP-IDF)

This component is a basic driver for controlling DC motors with H-Bridge using LEDC PWM.

## Features
- Control standard DC motors
- Suport two motor example left and right
- Simple and reusable

## How to Use
1. initialize the servo using :

```c
motor_driver_t driver = {
    .left  = { .in1 = 17, .in2 = 5, .pwm_pin = 18, .pwm_channel = LEDC_CHANNEL_0 },
    .right = { .in1 = 2, .in2 = 16, .pwm_pin = 4, .pwm_channel = LEDC_CHANNEL_1 },
    .pwm_timer      = LEDC_TIMER_0,
    .pwm_freq_hz    = 1000,
    .pwm_resolution = LEDC_TIMER_8_BIT
};

motor_init(&driver);
```

2. Controlling direction & speed :

``c
motor_forward(&driver, int speed);
motor_backward(&driver, int speed);
motor_turn_left(&driver, int speed);
motor_turn_right(&driver, int speed);
motor_stop(&driver);
```


## Notes
- This component is a basic library for control motors, created as part of the learning process on how to build modular components in ESP-IDF.
- Ensure your H-Bridge has a proper power supply.


