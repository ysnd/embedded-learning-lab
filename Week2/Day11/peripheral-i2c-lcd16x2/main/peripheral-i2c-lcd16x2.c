#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "hd44780.h"
#include "esp_err.h"

#define I2C_MASTER_NUM I2C_NUM_0
#define SDA_PIN 21
#define SCL_PIN 22
#define LCD_ADDR 0x27

void app_main(void)
{
    i2c_config_t conf = {
        .mode   = I2C_MODE_MASTER,
        .sda_io_num = SDA_PIN,
        .scl_io_num = SCL_PIN,
        .sda_pullup_en  = GPIO_PULLUP_ENABLE,
        .scl_pullup_en  = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000
    };
    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
    hd44780_t lcd = {
        .font   = HD44780_FONT_5X8,
        .lines  = 2
    };

    ESP_ERROR_CHECK(hd44780_init(&lcd));
    ESP_ERROR_CHECK(hd44780_clear(&lcd));
    ESP_ERROR_CHECK(hd44780_gotoxy(&lcd, 0, 0));
    ESP_ERROR_CHECK(hd44780_puts(&lcd, "Hello World!"));
    ESP_ERROR_CHECK(hd44780_gotoxy(&lcd, 0, 1));
    ESP_ERROR_CHECK(hd44780_puts(&lcd, "ESP-IDF I2C LCD"));

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    
}
