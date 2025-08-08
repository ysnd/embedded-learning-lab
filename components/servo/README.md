# Servo Component (ESP-IDF)

This component is a basic driver for controlling servo motors using LEDC PWM.

## Features
- Control standard hobby servo motors
- Suport multiple servos via struct-based approach
- Simple and reusable

## How to Use
1. initialize the servo using `servo_init()`

```c
servo_t my_servo;
servo_init(&my_servo, gpio_num, channel);
```
2. Set servo angle using `servo_write(&my_servo, 90); // Move to 90 degrees`

## Notes
- This component is a basic library for servo control, created as part of the learning process on how to build modular components in ESP-IDF.
- Ensure your servo has a proper power supply.
- Default configuration is for 50Hz PWM frequency with ~1-2ms pulse width range.


