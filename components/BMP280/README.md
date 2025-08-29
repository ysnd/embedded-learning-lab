# BMP280 Barometric Pressure Temperature Sensor Component (ESP-IDF)

This component is a basic driver for BMP280 barometric pressure temperature sensor using ESP-IDF.

## Features
- Read temperature value.
- Read pressure value.
- Read altitude value.
- Simple and reusable.

## How to Use
1. Add the component to project.
2. Include and configure in the project :

```c
#include "BMP280.h"

    bmp280_conf_t bmp = {
        .i2c_port = I2C_NUM_0,
        .sda_pin = 21,
        .scl_pin = 22,
        .freq_hz = 100000,
        .osrs_t = OVRSAMP_X16,
        .osrs_p = OVRSAMP_X16,
        .mode = PWR_MODE_NORMAL,
        .stby_t = STBY_T_1000,
        .filter = FLTR_4
    };
    bmp280_init(&bmp);

    while (1) {
        bmp280_update_raw();
        double temp = bmp280_get_temp();
        double press = bmp280_get_pressure();
        double alt = bmp280_get_altitude();

        printf("Temp : %.2f C | Pressure : %.2f hPa | Altitude : %.2f m\n", temp, press/100.0, alt);
    }
```

## Parameters Configuration

| Parameter | Mode |
| --- | --- |
| `.osrs_t`, `.osrs_p` | Oversampling temperature & pressure : `OVRSAMP_SKIP`, `OVRSAMP_X0`, `OVRSAMP_X2`, `OVRSAMP_X4`, `OVRSAMP_X8`, `OVRSAMP_X16` |
| `.mode` | Power mode : `PWR_MODE_SLEEP`, `PWR_MODE_FORCED`, `PWR_MODE_NORMAL` |
| `.stby_t` | Standby time : `STBY_T_-1_5`, `STBY_T_62_5`, `STBY_T_125`, `STBY_T_250`, `STBY_T_500`,`STBY_T_1000`, `STBY_T_2000`, `STBY_T_4000` |
| `.filter` | Filter : `FLTR_OFF`, `FLTR_1`, `FLTR_4`, `FLTR_8`, `FLTR_16` |

## Reference

| Function | Description |
| --- | --- |
| `bmp280_init(bmp280_conf_t *conf)` | Init BMP280 sensor & I2C |
| `void bmp280_update_raw()`| Read update raw sensor value |
| `bmp280_get_temp()` | Read temperature value in Celcius degree |
| `bmp280_get_pressure()` | Read pressure value in (Pa) |
| `bmp280_get_altitude()` | Read altitude value in (m) |


## Notes
- See example usage in `../components/example/bmp280_barometric_pressure_temperature_sensor/`
- This component is a basic library for BMP280 barometric pressure temperature sensor, created as part of the learning process on how to build modular components in ESP-IDF.








