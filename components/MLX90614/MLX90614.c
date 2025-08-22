#include "MLX90614.h"
#include "driver/i2c.h"

static i2c_port_t g_i2c_port;

void mlx_init(mlx_conf_t *cfg) {
    g_i2c_port = cfg->i2c_port;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = cfg->sda_pin,
        .scl_io_num = cfg->scl_pin,
        .master.clk_speed = cfg->freq_hz
    };
    i2c_param_config(cfg->i2c_port, &conf);
    i2c_driver_install(cfg->i2c_port, I2C_MODE_MASTER, 0, 0, 0);
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
    i2c_master_cmd_begin(g_i2c_port, cmd, pdMS_TO_TICKS(100));
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
