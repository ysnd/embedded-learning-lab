#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "SSD1306.h"
#include "BMP280.h"



void app_main(void)
{
    bmp280_conf_t bmp = {
        .i2c_port = I2C_NUM_0,
        .sda_pin = 21,
        .scl_pin = 22,
        .freq_hz = 400000,
        .osrs_t = OVRSAMP_X16,
        .osrs_p = OVRSAMP_X16,
        .mode = PWR_MODE_NORMAL,
        .stby_t = STBY_T_1000,
        .filter = FLTR_4
    };
    bmp280_init(&bmp);

    ssd1306_conf_t conf = {
        .i2c_port = I2C_NUM_0,
        .sda_pin = 21,
        .scl_pin = 22,
        .freq_hz = 400000
    };
    ssd1306_init(&conf);

    while (1) {
        ssd1306_clear_buffer();
        bmp280_update_raw();
        double temp = bmp280_get_temp();
        double press = bmp280_get_pressure();
        double alt = bmp280_get_altitude();
        double hpa = press / 100.0;

        printf("Temp : %.2f C | Pressure : %.2f hPa | Altitude : %.2f m\n", temp, hpa, alt);
        ssd1306_draw_string(0, 0, "---------------------");
        ssd1306_draw_string(0, 6, "ESP32 WEATHER STATION");
        ssd1306_draw_string(0, 9, "_____________________"); 
        char td[21]; 
        sprintf(td, "Temperature : %.2f", temp); 
        ssd1306_draw_string(0, 24, td);
        ssd1306_draw_string(121, 24, "C");
        char pd[20];
        sprintf(pd, "Pressure    : %.1f", hpa);
        ssd1306_draw_string(0, 36, pd);
        ssd1306_draw_string(112, 36, "hPa");
        char ad[20];
        sprintf(ad, "Altitude    : %.2f", alt);
        ssd1306_draw_string(0, 48, ad);
        ssd1306_draw_string(121, 48, "m");

        
        ssd1306_update_display();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

}
