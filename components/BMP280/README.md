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


void app_main(void)
{
    bmp280_conf_t bmp = {
        .i2c_port = I2C_NUM_0,
        .sda_pin = 21,
        .scl_pin = 22,
        .freq_hz = 100000
    };
    bmp280_init(&bmp);

    while (1) {
        bmp280_update_raw();
        double temp = bmp280_get_temp();
        double press = bmp280_get_pressure();
        double alt = bmp280_get_altitude();

        printf("Temp : %.2f C | Pressure : %.2f hPa | Altitude : %.2f m\n", temp, press/100.0, alt);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
```

## Reference

| Function | Description |
| --- | --- |
| `bmp280_init(bmp280_conf_t *conf)` | Init BMP280 sensor & I2C |
| `void bmp280_update_raw()`| Read update raw sensor value |
| `bmp280_get_temp()` | Read temperature value |
| `bmp280_get_pressure()` | Read pressure value |
| `bmp280_get_altitude()` | Read altitude value |

## Notes
- See example usage in `../components/example/bmp280_barometric_pressure_temperature_sensor/`
- This component is a basic library for BMP280 barometric pressure temperature sensor, created as part of the learning process on how to build modular components in ESP-IDF.








