# SSD1306 OLED I2C Display Component (ESP-IDF)

This component is a basic driver for SSD1306 OLED I2C Display using ESP-IDF.

## Features
- Display character.
- Simple and reusable.

## How to Use
1. Add the component to project.
2. Include and configure in the project :

```c
#include "SSD1306.h"

    ssd1306_conf_t conf = {
        .i2c_port = I2C_NUM_0,
        .sda_pin = 21,
        .scl_pin = 22,
        .freq_hz = 400000
    };
    ssd1306_init(&conf);


    while (1) {
        ssd1306_clear_buffer();
        ssd1306_draw_string(0, 0, "A C A B ! !");
        double temp = 123.45;
        char buf[16]; 
        sprintf(buf, "Temperature : %.2f", temp); 
        ssd1306_draw_string(0, 24, buf);
        ssd1306_update_display();
        vTaskDelay(pdMS_To_TICKS(1000))
    }
```

## Reference

| Function | Description |
| --- | --- |
| `ssd1306_init(ssd1306_conf_t *conf)` | Init SSD1306 display & I2C |
| `ssd1306_clear_buffer()`| Clear display |
| `ssd1306_draw_string(int x, int y, const char* str)` | Display character string |
| `ssd1306_update_display()` | Update character / string in display |



## Notes
- See example usage in `../components/example/ssd1306_oled_basic/`
- This component is a basic library for SSD1306 OLED I2C Display, created as part of the learning process on how to build modular components in ESP-IDF.








