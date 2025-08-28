#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "BMP280.h"

void app_main(void)
{
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
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
