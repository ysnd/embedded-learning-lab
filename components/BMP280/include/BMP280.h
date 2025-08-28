#pragma once
#include <stdint.h>

#define BMP280_ADDR 0x76

// Oversampling
#define OVRSAMP_SKIP   0x00
#define OVRSAMP_X1     0x01
#define OVRSAMP_X2     0x02
#define OVRSAMP_X4     0x03
#define OVRSAMP_X8     0x04
#define OVRSAMP_X16    0x05

// Power mode
#define PWR_MODE_SLEEP  0x00
#define PWR_MODE_FORCED 0x01
#define PWR_MODE_NORMAL 0x03

// Standby time
#define STBY_T_0_5    0x00
#define STBY_T_62_5   0x01
#define STBY_T_125    0x02
#define STBY_T_250    0x03
#define STBY_T_500    0x04
#define STBY_T_1000   0x05
#define STBY_T_2000   0x06
#define STBY_T_4000   0x07

// Filter
#define FLTR_OFF  0x00
#define FLTR_2    0x01
#define FLTR_4    0x02
#define FLTR_8    0x03
#define FLTR_16   0x04


typedef struct {
    uint8_t i2c_port;
    uint8_t sda_pin;
    uint8_t scl_pin;
    int freq_hz;
    
    uint8_t osrs_t;
    uint8_t osrs_p;
    uint8_t mode;
    uint8_t stby_t;
    uint8_t filter;
} bmp280_conf_t;

void bmp280_init(bmp280_conf_t *conf);
void bmp280_update_raw(void);
double bmp280_get_temp(void);
double bmp280_get_pressure(void);
double bmp280_get_altitude(void);


