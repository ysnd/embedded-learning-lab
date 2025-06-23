#include <stdio.h>
#include "driver/i2c.h"
#include "esp_log.h"


#define I2C_PORT I2C_NUM_0
#define I2C_SDA 21
#define I2C_SCL 22
#define I2C_FREQ 100000

#define MLX90614_ADDR 0x5A
#define MLX90614_REG_OBJ_TEMP 0x07
#define MLX90614_REG_AMB_TEMP 0x06

void i2c_master_init()
{
    i2c_config_t conf = {
        .mode   = I2C_MODE_MASTER,
        .sda_io_num = I2C_SDA,
        .scl_io_num = I2C_SCL,
        .sda_pullup_en  = GPIO_PULLUP_ENABLE,
        .scl_pullup_en  = GPIO_PULLUP_ENABLE,
        .master.clk_speed   = I2C_FREQ
    };
    i2c_param_config(I2C_PORT, &conf);
    i2c_driver_install(I2C_PORT, conf.mode, 0, 0, 0);
}

float read_temp(uint8_t reg) {
    uint8_t data[3]; // 2 byte data + 1 CRC (biasanya bisa diabaikan untuk uji coba)
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MLX90614_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MLX90614_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, data, 3, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);

    uint16_t raw = (data[1] << 8) | data[0];
    return (raw * 0.02) - 273.15;  // Celsius
}

void app_main(void) {
    i2c_master_init();
    while (1) {
        float ambient = read_temp(MLX90614_REG_AMB_TEMP);
        float object = read_temp(MLX90614_REG_OBJ_TEMP);
        printf("Ambient: %.2f°C, Object: %.2f°C\n", ambient, object);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
