# TCS34725 Color Sensor Component (ESP-IDF)

This component is a basic driver for TCS34725 color sensor using ESP-IDF.

## Features
- Read raw data clear, red, green, blue value 
- Simple and reusable

## How to Use
1. Add the component to project.
2. Include and configure in the project :

```c
#include "TCS34725.h"


void app_main(void)
{
    tcs34725_conf_t tcs = {
        .i2c_port = I2C_NUM_0,
        .sda_pin = SDA_PIN,
        .scl_pin = SCL_PIN,
        .freq_hz = 100000
    };
    tcs34725_init(&tcs);

    while (1) {
        uint16_t c, r, g, b;
        if (tcs34725_data_ready()) {
            tcs34725_read_rgbc(&c, &r, &g, &b);
            printf("C : %u | R : %u | G : %u | B : %u\n", c, r, g, b);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
```

## Reference

| Function | Description |
| --- | --- |
| `tcs34725_init(tcs34725_conf_t *conf);` | Init TCS34725 color sensor & I2C |
| `tcs34725_data_ready();`| Check measure result is ready? |
| `tcs34725_read_rgbc(&c, &r, &g, &b);` | Read c, r, g, b value |

## Notes
- See example usage in `../components/example/TCS34725_color_sensor_basic/`
- This component is a basic library for TCS34725 color sensor, created as part of the learning process on how to build modular components in ESP-IDF.








