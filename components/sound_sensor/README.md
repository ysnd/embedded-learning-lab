# Sound Sensor Component (ESP-IDF)

This component is a basic driver for Sound sensors.

## Features
- Detect sound 
- Suport multiple sensors via struct-based approach
- Simple and reusable

## How to Use
1. initialize the sensor using `sound_sensor_init()`

```c
sound_sensor_t sensor;
sound_sensor_init(&sensor, GPIO);
```
2. Read sensor state using `sound_sensor_is_detected();`

## Notes
- see example usage in `../components/example/sound_sensor_basic/`
- This component is a basic library for sound sensors, created as part of learning process on how to build modular components in ESP-IDF.


