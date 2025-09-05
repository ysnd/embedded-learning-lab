#include "MPU6050.h"
#include "driver/i2c.h"
#include "include/MPU6050.h"
#include <stdint.h>
#include <stdio.h>

static mpu6050_conf_t mpu_conf;

static void i2c_init(mpu6050_conf_t *conf) {
    i2c_config_t i2c_conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = conf->sda_pin,
        .scl_io_num = conf->scl_pin,
        .master.clk_speed = conf->freq_hz
    };

    i2c_param_config(conf->i2c_port, &i2c_conf);
    i2c_driver_install(conf->i2c_port, I2C_MODE_MASTER, 0, 0, 0);
}

static void mpu6050_write(uint8_t reg, uint8_t data) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MPU6050_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_write_byte(cmd, data, true);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(mpu_conf.i2c_port, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);
}

static void mpu6050_read(uint8_t reg, uint8_t *data, size_t len) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MPU6050_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MPU6050_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, data, len, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(mpu_conf.i2c_port, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);
}

void mpu6050_init(mpu6050_conf_t *conf) {
    mpu_conf = *conf;
    i2c_init(conf);
    mpu6050_write(PWR_MGMT_1, 0x01); //pakai PLL dengan x-axis gyro sebagai clock
    //set sample rate   
    mpu6050_write(SMPLRT_DIV, conf->smplrt_div); // 100hz | sample rate divider: freq = 8khz / 1+SMPLRT_DIV) jika DLPF off atau 1khz / (1+SMPLRT_DIV) jika DPLF on                     
   // set DPLF (low pass filter) 
    mpu6050_write(CONFIG, conf->dlpf_cfg & 0x07); //44hz | config : 0=260hz, 1= 184hz, 2=94hz 3=44hz, 4=21hz, 5=10hz, 6=5hz //set gyro full scale range 
    mpu6050_write(GYRO_CONFIG, (conf->gyro_fs & 0x03) << 3); // +-500dps | 0= +-250dps, 1=500, 2 = 1000dps, 3 = 2000dps 
    mpu6050_write(ACCEL_CONFIG, (conf->accel_fs & 0x03) << 3);  
}

void mpu6050_read_accel(float *ax, float *ay, float *az) {
    uint8_t buf[6];
    mpu6050_read(0x3B, buf, 6);
    int16_t raw_ax = (int16_t)((buf[0] << 8) | buf[1]);
    int16_t raw_ay = (int16_t)((buf[2] << 8) | buf[3]);
    int16_t raw_az = (int16_t)((buf[4] << 8) | buf[5]);

    float accel_scale;
    switch (mpu_conf.accel_fs) {
        case ACCEL_FS_2G: accel_scale = 16384.0; break;
        case ACCEL_FS_4G: accel_scale = 8192.0; break;
        case ACCEL_FS_8G: accel_scale = 4096.0; break;
        case ACCEL_FS_16G: accel_scale = 2048.0; break;
        default: accel_scale = 16384.0;
    }
    //hasil dalam satuan g 
    *ax = (float)raw_ax / accel_scale;
    *ay = (float)raw_ay / accel_scale;
    *az = (float)raw_az / accel_scale;
}

void mpu6050_read_gyro(float *gx, float *gy, float *gz) {
    uint8_t buf[6];
    mpu6050_read(0x43, buf, 6);
    int16_t raw_gx = (int16_t)((buf[0] << 8) | buf[1]);
    int16_t raw_gy = (int16_t)((buf[2] << 8) | buf[3]);
    int16_t raw_gz = (int16_t)((buf[4] << 8) | buf[5]);

    float gyro_scale;
    switch (mpu_conf.gyro_fs) {
        case GYRO_FS_250DPS: gyro_scale = 131.0; break;
        case GYRO_FS_500DPS: gyro_scale = 65.5; break;
        case GYRO_FS_1000DPS: gyro_scale = 32.8; break;
        case GYRO_FS_2000DPS: gyro_scale = 16.4; break;
        default: gyro_scale = 16.4;
    }
    *gx = (float)raw_gx / gyro_scale;
    *gy = (float)raw_gy / gyro_scale;
    *gz = (float)raw_gz / gyro_scale;
}

float mpu6050_read_temp() {
    uint8_t buf[2];
    int16_t raw_temp;
    mpu6050_read(0x41, buf, 2);
    raw_temp = (buf[0] << 8) | buf[1];
    return (raw_temp / 340.0) + 36.53;
}
