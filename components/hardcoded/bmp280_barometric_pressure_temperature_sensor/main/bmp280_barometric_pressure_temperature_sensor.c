#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_rom_sys.h"
#include "driver/i2c.h"
#include "hal/i2c_types.h"

#define I2C_PORT I2C_NUM_0
#define I2C_SDA 21
#define I2C_SCL 22
#define I2C_FREQ_HZ 100000
#define BMP280_ADDR 0x76

static void i2c_master_init(void) {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_SDA,
        .scl_io_num = I2C_SCL,
        .master.clk_speed = I2C_FREQ_HZ,
    };
    i2c_param_config(I2C_PORT, &conf);
    i2c_driver_install(I2C_PORT, I2C_MODE_MASTER, 0, 0, 0);
}

static esp_err_t i2c_write_reg(uint8_t reg, uint8_t value) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (BMP280_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_write_byte(cmd, value, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_PORT, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);
    return ret;
}

static esp_err_t i2c_read_regs(uint8_t reg, uint8_t *buf, size_t len) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (BMP280_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (BMP280_ADDR << 1) | I2C_MASTER_READ, true);
    if (len > 1) {
        i2c_master_read(cmd, buf, len - 1, I2C_MASTER_ACK);
    }
    i2c_master_read_byte(cmd, &buf[len - 1], I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_PORT, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);
    return ret;
}

typedef struct {
    uint16_t dig_T1;
    int16_t dig_T2, dig_T3;
    uint16_t dig_P1;
    int16_t dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
} bmp280_cal_t;

static bmp280_cal_t cal;
static int32_t t_fine;

static void bmp280_read_cal(void) {
    uint8_t buf[24];
    i2c_read_regs(0x88, buf, sizeof(buf));
    cal.dig_T1 = buf[0] | (buf[1] << 8);
    cal.dig_T2 = buf[2] | (buf[3] << 8);
    cal.dig_T3 = buf[4] | (buf[5] << 8);
    cal.dig_P1 = buf[6] | (buf[7] << 8);
    cal.dig_P2 = buf[8] | (buf[9] << 8);
    cal.dig_P3 = buf[10] | (buf[11] << 8);
    cal.dig_P4 = buf[12] | (buf[13] << 8);
    cal.dig_P5 = buf[14] | (buf[15] << 8);
    cal.dig_P6 = buf[16] | (buf[17] << 8);
    cal.dig_P7 = buf[18] | (buf[19] << 8);
    cal.dig_P8 = buf[20] | (buf[21] << 8);
    cal.dig_P9 = buf[22] | (buf[23] << 8);
}

static void bmp280_start_measurement(void)
{
    i2c_write_reg(0xF4, 0xB7);
    i2c_write_reg(0xF5, 0x1C);
}

static void bmp280_read_raw(int32_t *raw_press, int32_t *raw_temp) {
    uint8_t buf[6];
    i2c_read_regs(0xF7, buf, 6);
    *raw_press = ((int32_t)buf[0] << 12) | ((int32_t)buf[1] << 4) | ((buf[2] >> 4) & 0x0F);
    *raw_temp  = ((int32_t)buf[3] << 12) | ((int32_t)buf[4] << 4)  | ((buf[5] >> 4) & 0x0F);
}

static double bmp280_compensate_temp(int32_t raw_temp) {
    int32_t var1, var2;
    var1 = ((((raw_temp >> 3) - ((int32_t)cal.dig_T1 << 1))) * (int32_t)cal.dig_T2) >> 11;
    var2 = (((((raw_temp >> 4) - (int32_t)cal.dig_T1) * ((raw_temp >> 4) - (int32_t)cal.dig_T1)) >> 12) * (int32_t)cal.dig_T3) >> 14;
    t_fine = var1 + var2;
    return (t_fine * 5 + 128) >> 8;
}

static double bmp280_compensate_press(int32_t raw_press) {
    int64_t var1, var2, p;
    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)cal.dig_P6;
    var2 += (var1 * (int64_t)cal.dig_P5) << 17;
    var2 += ((int64_t)cal.dig_P4) << 35;
    var1 = ((var1 * var1 * (int64_t)cal.dig_P3) >> 8) + ((var1 * (int64_t)cal.dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1) * (int64_t)cal.dig_P1) >> 33;
    if (var1 == 0) return  0;   
    p = 1048576 - raw_press;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)cal.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)cal.dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)cal.dig_P7) << 4);
    return p / 256.0;
}

static double altitude_m(double press_pa) {
    return 44330.0 * (1.0 - pow(press_pa / 101325.0, 0.190295));
}


void app_main(void)
{
    i2c_master_init();
    uint8_t id;
    i2c_read_regs(0xD0, &id, 1);
    if (id != 0x58) {
        printf("BMP280 not found (ID = 0x%02X)\n", id);
        return;
    }
    bmp280_read_cal();
    bmp280_start_measurement();

    while (1) {
        int32_t raw_press, raw_temp;
        bmp280_read_raw(&raw_press, &raw_temp);

        double t = bmp280_compensate_temp(raw_temp) / 100.0;
        double p = bmp280_compensate_press(raw_press);
        double alt = altitude_m(p);

        printf("Temp : %.2f C | Pressure : %.2f hPa | Altitude : %.2f m\n", t, p /100.0, alt);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
