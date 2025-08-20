#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "lcd1602_i2c.h"

void app_main(void)
{
    lcd_conf_t my_lcd = {
        .addr = 0x027,
        .sda_pin = 21,
        .scl_pin = 22,
        .freq_hz = 100000
    };
    lcd_init(&my_lcd);
    while (1) {
        lcd_set_cursor(0, 0);
        lcd_print("ESP32 LCD 1602!");
        lcd_set_cursor(1, 0);
        lcd_print(" Hello World ! ");
        vTaskDelay(pdMS_TO_TICKS(1000));
        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_print(" Hello Polly ! ");
        lcd_set_cursor(1, 0);
        lcd_print(" How are you ? ");
        vTaskDelay(pdMS_TO_TICKS(1000));
        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_print(" This is Demo! ");
        lcd_set_cursor(1, 0);
        lcd_print(" (_)(_)====D-- ");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
