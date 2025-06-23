#include <stdio.h>
#include <string.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "esp_rom_sys.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "esp_log.h"



#define I2C_PORT I2C_NUM_0
#define LCD_ADDR 0x27
#define SDA_IO 21
#define SCL_IO 22
#define I2C_FREQ 100000
#define BACKLIGHT 0x08
#define ENABLE 0x04
#define RW 0x00
#define RS_CMD 0x00
#define RS_DATA 0x01

#define MLXADR 0x5A
#define MLX_AMB_REG 0x06
#define MLX_OBJ_REG 0x07

void lcd_write_byte(uint8_t data) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (LCD_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, data | BACKLIGHT, true);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_PORT, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);
}

void lcd_togle_enable(uint8_t data) {
    lcd_write_byte(data | ENABLE);
    esp_rom_delay_us(500);
    lcd_write_byte(data & ~ENABLE);
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

void lcd_init() {
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

float mlx_read_temp(uint8_t reg) {
    uint8_t data[3];
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (MLXADR << 1) | I2C_MASTER_WRITE, true);
        i2c_master_write_byte(cmd, reg, true);
        i2c_master_start(cmd); // repeated start
        i2c_master_write_byte(cmd, (MLXADR << 1) | I2C_MASTER_READ, true);
        i2c_master_read(cmd, data, 3, I2C_MASTER_LAST_NACK);
        i2c_master_stop(cmd);
        esp_err_t ret = i2c_master_cmd_begin(I2C_PORT, cmd, pdMS_TO_TICKS(100));
        i2c_cmd_link_delete(cmd);

        if (ret !=ESP_OK) {
            printf("I2C Read Failed\n");
            return -1;
        }
        uint16_t raw = (data[1] << 8) | data[0];
        return (float)raw * 0.02 - 273.15;
}

void app_main(void) {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = SDA_IO,
        .scl_io_num = SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_FREQ,
    };
    i2c_param_config(I2C_PORT, &conf);
    i2c_driver_install(I2C_PORT, conf.mode,0 ,0 ,0);

    lcd_init();
    while (1) {
        float amb = mlx_read_temp(MLX_AMB_REG);
        float obj = mlx_read_temp(MLX_OBJ_REG);

        char buffer[17];
        lcd_set_cursor(0, 0);
        snprintf(buffer, sizeof(buffer),"Ambient : %.2fC", amb);
        lcd_print(buffer);
        lcd_set_cursor(1, 0);
        snprintf(buffer, sizeof(buffer), "Object : %.2fC", obj);
        lcd_print(buffer);
        vTaskDelay(pdMS_TO_TICKS(200));
    }

}

