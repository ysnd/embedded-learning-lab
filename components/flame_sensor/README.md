# Flame Sensor Component (ESP-IDF)

This component is a basic driver for flame sensors.

## Features
- Detect flame 
- Suport multiple sensors via struct-based approach
- Simple and reusable

## How to Use
1. initialize the sensor using `flame_sensor_init()`

```c
flame_sensor_t sensor;
flame_sensor_init(&sensor, GPIO);
```
2. Read sensor state using `flame_sensor_is_detected();`

## Notes
- see example usage in `../components/example/flame_sensor_basic/`
- This component is a basic library for flame sensors, created as part of learning process on how to build modular components in ESP-IDF.



