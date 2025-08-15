
# PIR Sensor Component (ESP-IDF)

This component is a basic driver for PIR sensors.

## Features
- Detect motion
- Suport multiple sensors via struct-based approach
- Simple and reusable

## How to Use
1. initialize the sensor using `pir_sensor_init()`

```c
pir_sensor_t sensor;
pir_sensor_init(&sensor, pin);
```
2. Read sensor state using `pir_sensor_is_motiondetected();`

## Notes
- see example usage in `../components/example/pir_sensor_basic/`
- This component is a basic library for PIR sensors, created as part of learning process on how to build modular components in ESP-IDF.

