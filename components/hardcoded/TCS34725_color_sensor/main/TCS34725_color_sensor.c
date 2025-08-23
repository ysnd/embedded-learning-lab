#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"


#define I2C_PORT I2C_NUM_0
#define SDA_PIN 21
#define SCL_PIN 22
#define TCS_ADDR 0x29
#define REG_EN 0x80 | 0x00
#define REG_ATIME 0x81 | 0x00
#define REG_GAIN 0x8F | 0x00
#define REG_STATUS 0x93 | 0x00
#define REG_CDATAL 0x94 | 0xA0


void i2c_init() {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = SDA_PIN,
        .scl_io_num = SCL_PIN,
        .master.clk_speed = 100000
    };
    i2c_param_config(I2C_PORT, &conf);
    i2c_driver_install(I2C_PORT, I2C_MODE_MASTER, 0, 0, 0);
}

// write 8bit ke register
void write8(uint8_t reg, uint8_t value) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (TCS_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_write_byte(cmd, value, true);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_PORT, cmd, pdMS_TO_TICKS(100));
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
    i2c_master_cmd_begin(I2C_PORT, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);
    return value;
}

void tcs34725_init() {
    write8(REG_EN, 0x03);
    write8(REG_ATIME, 0xD6);
    write8(REG_GAIN, 0x01);
}

static bool data_ready(void) {
    return (read8(REG_STATUS) & 0x01) != 0;
}

static void read_rgbc(uint16_t *c, uint16_t *r, uint16_t *g, uint16_t *b) {
    uint8_t buf[8];
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (TCS_ADDR <<1 ) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, REG_CDATAL, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (TCS_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, buf, 8, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_PORT, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);

    *c = buf[0] | (buf[1] << 8);
    *r = buf[2] | (buf[3] << 8);
    *g = buf[4] | (buf[5] << 8);
    *b = buf[6] | (buf[7] << 8);
}

void app_main(void)
{
    
    i2c_init();
    tcs34725_init();
    vTaskDelay(pdMS_TO_TICKS(100));

    while (1) {
        uint16_t c, r, g, b;
        if (data_ready()) {
            read_rgbc(&c, &r, &g, &b);
			printf("C=%u R=%u G=%u B=%u\n", c, r, g, b);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
