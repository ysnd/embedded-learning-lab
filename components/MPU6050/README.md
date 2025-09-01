# MPU6050 Accelerometer Gyroscope Sensor Component (ESP-IDF)

This component is a basic driver for MPU6050 Accelerometer Gyroscope sensor using ESP-IDF.

## Features
- Read accelerometer value.
- Read gyroscope value.
- Read internal temperature value.
- Simple and reusable.

## How to Use
1. Add the component to project.
2. Include and configure in the project :

```c
#include "MPU6050.h"

    mpu6050_conf_t mpu = {
        .i2c_port = I2C_NUM_0,
        .sda_pin = 21,
        .scl_pin = 22,
        .freq_hz = 100000,
    };
    mpu6050_init(&mpu);

    while (1) {
        ..
    }
```

## Parameters Configuration

| Parameter | Mode |
| --- | --- |
| ``, `` |  |


## Reference

| Function | Description |
| --- | --- |
| `mpu6050_init(mpu6050_conf_t *conf)` | Init MPU6050 sensor & I2C |
| `mpu6050_read_accel(&ax, &ay, &az)`| Read accelerometer value  |
| `mpu6050_read_gyro(&gx, &gy, &gz)` | Read gyroscope value |
| `mpu6050_read_temp();` | Read internal sensor temperature |


## Notes
- See example usage in `../components/example/mpu6050_accel_gyro_sensor/`
- This component is a basic library for mpu6050 accelerometer gyroscope sensor, created as part of the learning process on how to build modular components in ESP-IDF.









