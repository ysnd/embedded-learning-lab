#pragma once
#include <stdint.h>

#define BMP280_ADDR 0x76

typedef struct {
    uint8_t i2c_port;
    uint8_t sda_pin;
    uint8_t scl_pin;
    int freq_hz;
} bmp280_conf_t;

void bmp280_init(bmp280_conf_t *conf);
void bmp280_update_raw(void);
double bmp280_get_temp(void);
double bmp280_get_pressure(void);
double bmp280_get_altitude(void);


