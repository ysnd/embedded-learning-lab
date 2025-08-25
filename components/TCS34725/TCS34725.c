#include "TCS34725.h"
#include "driver/i2c.h"
#include "include/TCS34725.h"

static tcs34725_conf_t tcs_conf;

void i2c_init(tcs34725_conf_t *conf) {
    i2c_config_t i2c_conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = conf->sda_pin,
        .scl_io_num = conf->scl_pin,
        .master.clk_speed = conf->freq_hz
    };
    i2c_param_config(conf->i2c_port, &i2c_conf);
    i2c_driver_install(conf->i2c_port, I2C_MODE_MASTER, 0, 0, 0);
}

// write 8bit ke register
void write8(uint8_t reg, uint8_t value) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (TCS_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_write_byte(cmd, value, true);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(tcs_conf.i2c_port, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);
}

//read 16bit dari register(low,high)
uint8_t read8(uint8_t reg) {
    uint8_t value;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (TCS_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (TCS_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd, &value, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(tcs_conf.i2c_port, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);
    return value;
}

bool tcs34725_data_ready(void) {
    return (read8(REG_STATUS) & 0x01) != 0;
}

void tcs34725_init(tcs34725_conf_t *conf) {
    tcs_conf = *conf;
    i2c_init(conf);

    write8(REG_EN, 0x03); 
    write8(REG_ATIME, 0xD6); 
    write8(REG_GAIN, 0x01); 
    vTaskDelay(pdMS_TO_TICKS(3));
}


void tcs34725_read_rgbc(uint16_t *c, uint16_t *r, uint16_t *g, uint16_t *b) {
    uint8_t buf[8];
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (TCS_ADDR <<1 ) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, REG_CDATAL, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (TCS_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, buf, 8, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(tcs_conf.i2c_port, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);

    *c = buf[0] | (buf[1] << 8);
    *r = buf[2] | (buf[3] << 8);
    *g = buf[4] | (buf[5] << 8);
    *b = buf[6] | (buf[7] << 8);
}
