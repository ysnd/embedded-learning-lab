#include "lcd1602_i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_rom_sys.h"
#include "driver/i2c.h"
#include "include/lcd1602_i2c.h"

static const lcd_conf_t *cfg = NULL;

#define BL 0x08
#define EN 0x04
#define RW 0x00
#define RS_CMD 0x00
#define RS_DATA 0x01

void lcd_write_byte(uint8_t data) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (cfg->addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, data | BL, true);
    i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);
}

void lcd_togle_enable(uint8_t data) {
    lcd_write_byte(data | EN);
    esp_rom_delay_us(500);
    lcd_write_byte(data & ~EN);
    esp_rom_delay_us(500);
}

void lcd_send_nibble(uint8_t nibble, bool rs) {
    uint8_t data = (nibble << 4) | (rs ? RS_DATA : RS_CMD);
    lcd_write_byte(data);
    lcd_togle_enable(data);
}

void lcd_send_byte(uint8_t byte, bool rs) {
    lcd_send_nibble(byte >> 4, rs);
    lcd_send_nibble(byte & 0x0F, rs);
}

void lcd_command(uint8_t cmd) {
    lcd_send_byte(cmd, false);
}

void lcd_data(uint8_t data) {
    lcd_send_byte(data, true);
}

void lcd_init(const lcd_conf_t *c) {
    cfg = c;
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = cfg->sda_pin,
        .scl_io_num = cfg->scl_pin,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = cfg->freq_hz
    };
    i2c_param_config(I2C_NUM_0, &conf);
    i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0);

    vTaskDelay(pdMS_TO_TICKS(50));
    lcd_send_nibble(0x03, false);
    vTaskDelay(pdMS_TO_TICKS(5));
    lcd_send_nibble(0x03, false);
    vTaskDelay(pdMS_TO_TICKS(5));
    lcd_send_nibble(0x03, false);
    vTaskDelay(pdMS_TO_TICKS(5));
    lcd_send_nibble(0x02, false);

    lcd_command(0x28);
    lcd_command(0x0C);
    lcd_command(0x06);
}

void lcd_clear() {
    lcd_command(0x01);
    vTaskDelay(pdMS_TO_TICKS(2));
}

void lcd_set_cursor(uint8_t row, uint8_t col) {
    uint8_t offsets[] = {0x00, 0x40};
    lcd_command(0x80 | (offsets[row] + col));
}

void lcd_print(const char* str) {
    while (*str) {
        lcd_data((uint8_t)*str++);
    }
}
