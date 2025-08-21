#include <stdint.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "hal/i2c_types.h"

#define MLX_ADDR 0x5A
#define I2C_PORT I2C_NUM_0

void i2c_master_init() {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = 21,
        .scl_io_num = 22,
        .master.clk_speed = 100000
    };
    i2c_param_config(I2C_PORT, &conf);
    i2c_driver_install(I2C_PORT, I2C_MODE_MASTER, 0, 0, 0);
}

float read_temp(uint8_t reg) {
    uint8_t data[3];
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MLX_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MLX_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, data, 3, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_PORT, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);

    uint16_t raw = (data[1] << 8) | data[0];
    return (raw*0.02) - 273.15;
}

float mlx_read_temperature_ambient() {
   return read_temp(0x06);
}

float mlx_read_temperature_object() {
   return read_temp(0x07);
}

void app_main(void)
{ 
    i2c_master_init();
    while (1) {
        float ta = mlx_read_temperature_ambient();
        float to = mlx_read_temperature_object();
        printf("Ambient : %.2f Celcius\n", ta);
        printf("Object  : %.2f Celcius\n", to);
                vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
