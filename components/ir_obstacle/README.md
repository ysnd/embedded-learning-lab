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
```
2. Read sensor state using `ir_obstacle_read();`

## Notes
- Sensor output is assumed to be *active LOW* when obstacle is detected.
- see example usage in `../components/example/ir_obstacle_basic/`
- This component is a basic library for IR obstacle sensors, created as part of learning process on how to build modular components in ESP-IDF.

