# LCD1602 I2C Component (ESP-IDF)

This component is a basic driver for LCD1602 I2C using ESP-IDF.

## Features
- Display character
- Backlight always on
- Simple and reusable

## How to Use
1. Add the component to project.
2. Include and configure in the project :

```c
#Include "lcd1602_i2c.h"

lcd_conf_t my_lcd = {
    .addr = LCD_I2C_ADDRESS, // 0x27 or 0x3F
    .sda_pin = SDA_PIN,
    .scl_pin = SCL_PIN,
    .freq_hz = 100000
};

void app_main(void)
{
    lcd_init(&my_lcd);
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print("Hello World!");
    lcd_set_cursor(0, 1);
    lcd_print("ESP32 1602 LCD");
}
```

## Reference

| Function | Description |
| --- | --- |
| `lcd_init(const lcd_conf_t *cfg);` | Init I2C bus and LCD |
| `lcd_clear();` | Clear the screen |
| `lcd_set_cursor(uint8_t row, uint8_t col);` | row 0-1, col 0-15 |
| `lcd_print(const char *str);` | Write text to display |

## Notes
- See example usage in `../components/example/lcd1602_i2c_basic/`
- This component is a basic library for LCD1602 I2C, created as part of the learning process on how to build modular components in ESP-IDF.






