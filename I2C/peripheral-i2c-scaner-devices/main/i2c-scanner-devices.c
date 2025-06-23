#include <stdio.h>
#include "driver/i2c.h"
#include "esp_err.h"

#define I2C_MSTR_NUM I2C_NUM_0
#define I2C_MSTR_SCL_IO 22
#define I2C_MSTR_SDA_IO 21
#define I2C_MSTR_FREQ_HZ 100000

void app_main(void)
{
    i2c_config_t conf = {
        .mode   = I2C_MODE_MASTER,
        .sda_io_num = I2C_MSTR_SDA_IO,
        .scl_io_num = I2C_MSTR_SCL_IO,
        .sda_pullup_en  = GPIO_PULLUP_ENABLE,
        .scl_pullup_en  = GPIO_PULLUP_ENABLE,
        .master.clk_speed   = I2C_MSTR_FREQ_HZ
    };
    i2c_param_config(I2C_MSTR_NUM, &conf);
    i2c_driver_install(I2C_MSTR_NUM, conf.mode, 0, 0, 0);

    printf("Scanning I2C devices..\n");
    for (uint8_t address = 1; address < 127; address++) {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, true);
        i2c_master_stop(cmd);
        esp_err_t ret = i2c_master_cmd_begin(I2C_MSTR_NUM, cmd, 100 / portTICK_PERIOD_MS);
        if (ret == ESP_OK) {
            printf("I2C device found at adress 0x%02X\n", address);
        } 
    }
    printf("Scan done.\n");
}
