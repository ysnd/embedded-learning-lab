#include <stdint.h>
#include <stdio.h>
#include "MPU6050.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    mpu6050_conf_t conf = {
        .i2c_port = I2C_NUM_0,
        .sda_pin = 21,
        .scl_pin = 22,
        .freq_hz = 100000,
        .smplrt_div = 9,
        .dlpf_cfg = DLPF_44HZ,
        .gyro_fs = GYRO_FS_500DPS,
        .accel_fs = ACCEL_FS_4G
    };
    mpu6050_init(&conf);

    float ax, ay, az, gx, gy, gz, temp;
    
    while (1) {
        mpu6050_read_accel(&ax, &ay, &az);
        mpu6050_read_gyro(&gx, &gy, &gz);
        temp = mpu6050_read_temp();


        printf("Accel : X=%.2f Y=%.2f Z=%.2f | Gyro : X=%.2f Y=%.2f Z=%.2f | Temp : %.2f C\n", ax, ay, az, gx, gy, gz, temp);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
