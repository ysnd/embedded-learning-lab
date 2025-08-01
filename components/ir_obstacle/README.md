# IR Obstacle Sensor Component (ESP-IDF)

This component is a basic driver for IR obstacle sensors.

## Features
- Detect obstacle 
- Suport multiple sensors via struct-based approach
- Simple and reusable

## How to Use
1. initialize the sensor using `ir_obstacle_init()`

```c
ir_obstacle_t sensor;
ir_obstacle_init(&sensor, GPIO);

2. Read sensor state using `ir_obstacle_read();`

## Notes
-Sensor output is assumed to be *active LOW* when obstacle is detected.
- This component is designed for educational purposes, especially to understand how to create a custom component for ESP-IDF.

