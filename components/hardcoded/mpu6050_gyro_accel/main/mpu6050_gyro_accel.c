#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"

#define I2C_PORT I2C_NUM_0
#define SDA_PIN 21
#define SCL_PIN 22
#define I2C_FREQ_HZ 400000
#define MPU6050_ADDR 0x68

#define PWR_MGMT_1   0x6B
#define SMPLRT_DIV   0x19
#define CONFIG       0x1A
#define GYRO_CONFIG  0x1B
#define ACCEL_CONFIG 0x1C

static void i2c_init() {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = SDA_PIN,
        .scl_io_num = SCL_PIN,
        .master.clk_speed = I2C_FREQ_HZ
    };

    i2c_param_config(I2C_PORT, &conf);
    i2c_driver_install(I2C_PORT, conf.mode, 0, 0, 0);
}

static void mpu6050_write(uint8_t reg, uint8_t data) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MPU6050_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_write_byte(cmd, data, true);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_PORT, cmd, pdMS_TO_TICKS(100));
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
    i2c_master_cmd_begin(I2C_PORT, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);
}

static void mpu6050_init() {
    mpu6050_write(PWR_MGMT_1, 0x01); //pakai PLL dengan x-axis gyro sebagai clock
    //set sample rate   
    mpu6050_write(SMPLRT_DIV, 9); // 100hz | sample rate divider: freq = 8khz / 1+SMPLRT_DIV) jika DLPF off atau 1khz / (1+SMPLRT_DIV) jika DPLF on                     
    // set DPLF (low pass filter)
    mpu6050_write(CONFIG, 3); //44hz | config : 0=260hz, 1= 184hz, 2=94hz 3=44hz, 4=21hz, 5=10hz, 6=5hz  
    //set gyro full scale range
    mpu6050_write(GYRO_CONFIG, (1 << 3)); // +-500dps | 0= +-250dps, 1=500, 2 = 1000dps, 3 = 2000dps
}

static void mpu6050_read_accel(int16_t *ax, int16_t *ay, int16_t *az) {
    uint8_t buf[6];
    mpu6050_read(0x3B, buf, 6);
    *ax = (buf[0] << 8) | buf[1];
    *ay = (buf[2] << 8) | buf[3];
    *az = (buf[4] << 8) | buf[5];
}

static void mpu6050_read_gyro(int16_t *gx, int16_t *gy, int16_t *gz) {
    uint8_t buf[6];
    mpu6050_read(0x43, buf, 6);
    *gx = (buf[0] << 8) | buf[1];
    *gy = (buf[2] << 8) | buf[3];
    *gz = (buf[4] << 8) | buf[5];
}

static float mpu6050_read_temp() {
    uint8_t buf[2];
    int16_t raw_temp;
    mpu6050_read(0x41, buf, 2);
    raw_temp = (buf[0] << 8) | buf[1];
    return (raw_temp / 340.0) + 36.53;
}

void app_main(void)
{
    i2c_init();
    mpu6050_init();
    while (1) {
        int16_t ax,ay,az,gx,gy,gz;
        float temp_c = mpu6050_read_temp();
        mpu6050_read_accel(&ax, &ay, &az);
        mpu6050_read_gyro(&gx, &gy, &gz);
        printf("Accel: X=%4d Y=%4d Z=%4d | Gyro: X=%4d Y=%4d Z=%4d | Temp: %.2f C\n", ax, ay, az, gx, gy, gz, temp_c);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
