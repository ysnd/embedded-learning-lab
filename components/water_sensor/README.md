# Water Sensor Component (ESP-IDF)

This component is a basic driver for water sensors.

## Features
- Detect water 
- Suport multiple sensors via struct-based approach
- Simple and reusable

## How to Use
1. initialize the sensor using `water_sensor_init()`

```c
water_sensor_t sensor;
water_sensor_init(&sensor, GPIO);
```
2. Read sensor state using `water_sensor_is_detected();`

## Notes
- see example usage in `../components/example/water_sensor_basic/`
- This component is a basic library for water sensors, created as part of learning process on how to build modular components in ESP-IDF.

