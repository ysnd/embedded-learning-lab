# Ultrasonic Sensor Component (ESP-IDF)

This component is a basic driver for ultrasonic distance sensor HCSR04/HY-SRF05 using ESP-IDF.

## Features
- Measure distance using ultrasonic time-of-flight
- Suport multiple sensors via struct-based approach
- Simple and reusable
- Timeout protection to avoid blocking forever

## How to Use
### Single Sensor
1. initialize and read directly, no struct neded:
`ultrasonic_sensor_init(GPIO_NUM_5, GPIO_NUM_18);`
2. Read sensor using 
`float distance = ultrasonic_sensor_read_cm();`

### Multiple Sensor
Use struct for multiple sensor suport:
```c
ultrasonic_sensor_t sensor;
ultrasonic_multiple_sensor_init(&sensor, GPIO_NUM_5, GPIO_NUM_18);
float distance = ultrasonic_multiple_read_cm(&sensor);
```

## Notes
- see example usage in `../components/example/ultrasonic_basic/`
- Timeout is 100ms to prevent lockup if echo not received.
- This component is a basic library for water sensors, created as part of learning process on how to build modular components in ESP-IDF.

