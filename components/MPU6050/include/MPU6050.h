#pragma once
#include <stdint.h>
#include "driver/i2c.h"

#define PWR_MGMT_1   0x6B
#define SMPLRT_DIV   0x19
#define CONFIG       0x1A
#define GYRO_CONFIG  0x1B
#define ACCEL_CONFIG 0x1C
#define MPU6050_ADDR 0x68

typedef enum {
    SMPLRT_1KHZ_DIV1 = 0,
    SMPLRT_1KHZ_DIV10 = 9,
    SMPLRT_1KHZ_DIV19 = 19,
} mpu6050_smplrt_t;

typedef enum {
    DLPF_260HZ = 0,
    DLPF_184HZ = 1,
    DLPF_94HZ = 2,
    DLPF_44HZ = 3,
    DLPF_21HZ = 4,
    DLPF_10HZ = 5,
    DLPF_5HZ = 6 
} mpu6050_dlpf_t;

typedef enum {
    GYRO_FS_250DPS = 0,
    GYRO_FS_500DPS = 1,
    GYRO_FS_1000DPS = 2,
    GYRO_FS_2000DPS = 3 
} mpu6050_gyro_fs_t;

typedef enum {
    ACCEL_FS_2G = 0,
    ACCEL_FS_4G = 1,
    ACCEL_FS_8G = 2,
    ACCEL_FS_16G = 3 
} mpu6050_accel_fs_t;

typedef struct {
    uint8_t i2c_port;
    gpio_num_t sda_pin;
    gpio_num_t scl_pin;
    uint32_t freq_hz;
    uint8_t smplrt_div; // sample rate divider
    mpu6050_dlpf_t dlpf_cfg; // digital low pass filter
    mpu6050_gyro_fs_t gyro_fs; //gyro full scale 250/500/1000/2000 dps 
    mpu6050_accel_fs_t accel_fs; //accelero full scale
} mpu6050_conf_t;

void mpu6050_init(mpu6050_conf_t *conf);
void mpu6050_read_accel(float *ax, float *ay, float *az);
void mpu6050_read_gyro(float *gx, float *gy, float *gz);
float mpu6050_read_temp();
