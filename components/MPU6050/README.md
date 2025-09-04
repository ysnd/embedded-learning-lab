# MPU6050 Accelerometer Gyroscope Sensor Component (ESP-IDF)

This component is a basic driver for MPU6050 Accelerometer Gyroscope sensor using ESP-IDF.

## Features
- Read accelerometer value in g.
- Read gyroscope value in degree/s.
- Read internal temperature in Celsius.
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
        .smplrt_div = SMPLRT_1KHZ_DIV10, // sample rate divider
        .dlpf_cfg = DLPF_44HZ,           // digital low pass filter
        .gyro_fs = GYRO_FS_500DPS,       // gyro full-scale range
        .accel_fs = ACCEL_FS_2G           // accelerometer full-scale range
    };
    mpu6050_init(&mpu);

    while (1) {
    float ax, ay, az;
    float gx, gy, gz;
    float temp;

    mpu6050_read_accel(&ax, &ay, &az);
    mpu6050_read_gyro(&gx, &gy, &gz);
    temp = mpu6050_read_temp();

    printf("Accel: X=%.2f Y=%.2f Z=%.2f | Gyro: X=%.2f Y=%.2f Z=%.2f | Temp: %.2f C\n",
           ax, ay, az, gx, gy, gz, temp);

    vTaskDelay(pdMS_TO_TICKS(100));
    }
```

## Parameters Configuration

| Parameter | Mode |
| --- | --- |
| `.smplrt_div` | Sample rate divider :`SMPLRT_1KHZ_DIV1`, `SMPLRT_1KHZ_DIV10`, `SMPLRT_1KHZ_DIV19` |
| `.dlpf_cfg` | Digital Low Pass Filter : `DLPF_260HZ`, `DLPF_184HZ`, `DLPF_94HZ`, `DLPF_44HZ`, `DLPF_21HZ`, `DLPF_10HZ`, `DLPF_5HZ` |
| `.gyro_fs` | Gyroscope full-scale range : `GYRO_FS_250DPS`, `GYRO_FS_500DPS`, `GYRO_FS_1000DPS`, `GYRO_FS_2000DPS` |
| `.accel_fs` | Accelerometer full-scale range : `ACCEL_FS_2G`, `ACCEL_FS_4G`, `ACCEL_FS_8G`, `ACCEL_FS_16G` |


## Reference

| Function | Description |
| --- | --- |
| `mpu6050_init(mpu6050_conf_t *conf)` | Init MPU6050 sensor & I2C |
| `mpu6050_read_accel(&ax, &ay, &az)`| Read accelerometer values in g  |
| `mpu6050_read_gyro(&gx, &gy, &gz)` | Read gyroscope values in degree/s |
| `mpu6050_read_temp();` | Read internal sensor temperature |


## Notes
- See example usage in `../components/example/mpu6050_accel_gyro_sensor/`
- This component is a basic library for mpu6050 accelerometer gyroscope sensor, created as part of the learning process on how to build modular components in ESP-IDF.









